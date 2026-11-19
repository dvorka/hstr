/*
 hstr_history.c     loading and processing of BASH history

 Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "include/hstr_history.h"

#include <errno.h>

#define NDEBUG
#include <assert.h>

typedef struct {
    char* item;
    unsigned rank;
} RankedHistoryItem;

static HistoryItems* prioritizedHistory;
static bool dirty;

#ifdef DEBUG_RADIX
#define DEBUG_RADIXSORT() radixsort_stat(&rs, false); exit(0)
#else
#define DEBUG_RADIXSORT()
#endif

// TODO make this configurable from command line as option
#define METRICS_LOGARITHM(RANK,ORDER,LENGTH) RANK+(log(ORDER)*10.0)+LENGTH
#define METRICS_ADDITIVE(RANK,ORDER,LENGTH)  RANK+ORDER/10+LENGTH

unsigned history_ranking_function(unsigned rank, int newOccurenceOrder, size_t length) {
    long metrics=METRICS_LOGARITHM(rank, newOccurenceOrder, length);
    assert(metrics<UINT_MAX);
    return metrics;
}

char* get_shell_history_file_name(void)
{
    // return allocated name of the shell history file: HISTFILE or shell specific default
    char* historyFile = hstr_getenv_strdup(ENV_VAR_HISTFILE);
    if(!historyFile) {
        if(is_zsh_parent_shell()) {
            historyFile = get_home_file_path(FILE_ZSH_HISTORY);
            if(access(historyFile, F_OK) == -1) {
                free(historyFile);
                historyFile = get_home_file_path(FILE_ZSH_ZHISTORY);
                // ... fallback if this file doesn't exist?
            }
        } else {
            historyFile = get_home_file_path(FILE_DEFAULT_HISTORY);
        }
    }
    return historyFile;
}

char* get_history_file_name(void)
{
    char* historyFile = hstr_getenv_strdup(ENV_VAR_HSTR_HISTFILE);
    return historyFile ? historyFile : get_shell_history_file_name();
}

void dump_prioritized_history(HistoryItems *historyItems)
{
    printf("\n\nPrioritized history:");
    unsigned i;
    for(i=0; i<historyItems->count; i++) {
        if(historyItems->items[i]!=NULL) {
            printf("\n%s",historyItems->items[i]); fflush(stdout);
        } else {
            printf("\n %d NULL",i); fflush(stdout);
        }
    }
    printf("\n"); fflush(stdout);
}

bool is_hist_timestamp(const char* line)
{
    // HISTTIMEFORMAT defined > ^#1234567890$

    if(line[0] != '#') {
        return false;
    }

    int i;
    for(i = 1; line[i] != '\0'; ++i) {
        if(!isdigit((unsigned char)line[i])) {
            return false;
        }
    }

    return (i >= 11);
}

static void history_mgmt_clear_history(void)
{
    HIST_ENTRY** list = history_list();
    if(list) {
        for(int i = 0; i < history_length; i++) {
            if(list[i] && list[i]->data) {
                free(list[i]->data);
                list[i]->data = NULL;
            }
        }
    }
    clear_history();
}

static void history_entry_set_timestamp(const char* timestamp)
{
    if(!timestamp) {
        return;
    }
    HIST_ENTRY** list = history_list();
    if(list && history_length > 0 && list[history_length - 1]) {
        list[history_length - 1]->data = hstr_strdup(timestamp);
    }
}

static bool append_entry_line(char** buf, size_t* len, size_t* cap, const char* line, size_t lineLen, bool hasLines)
{
    size_t needed = *len + (hasLines ? 1 : 0) + lineLen + 1;
    if(*cap < needed) {
        size_t newCap = (*cap > 0) ? *cap : 256;
        while(newCap < needed) {
            newCap *= 2;
        }
        char* grown = realloc(*buf, newCap);
        if(!grown) {
            return false;
        }
        *buf = grown;
        *cap = newCap;
    }
    if(hasLines) {
        (*buf)[(*len)++] = '\n';
    }
    if(lineLen > 0) {
        memcpy(*buf + *len, line, lineLen);
        *len += lineLen;
    }
    (*buf)[*len] = '\0';
    return true;
}

static bool load_bash_history_file(const char* historyFile)
{
    FILE* f = fopen(historyFile, "rb");
    if(!f) {
        fprintf(stderr, "\nUnable to read history file from '%s'!\n", historyFile);
        return false;
    }

    size_t cap = 4096;
    size_t totalLen = 0;
    char* buf = malloc(cap);
    if(!buf) {
        fclose(f);
        return false;
    }

    for(;;) {
        if(totalLen + 4096 + 1 > cap) {
            size_t newCap = cap * 2;
            char* grown = realloc(buf, newCap);
            if(!grown) {
                free(buf);
                fclose(f);
                return false;
            }
            buf = grown;
            cap = newCap;
        }
        size_t n = fread(buf + totalLen, 1, cap - totalLen - 1, f);
        totalLen += n;
        if(n == 0) {
            if(ferror(f)) {
                free(buf);
                fclose(f);
                return false;
            }
            break;
        }
    }
    fclose(f);
    buf[totalLen] = '\0';

    if(totalLen == 0) {
        free(buf);
        return true;
    }

    bool inTimestamped = false;
    const char* pendingTimestamp = NULL;
    char* entryBuf = NULL;
    size_t entryLen = 0;
    size_t entryCap = 0;
    bool hasEntryLines = false;

    size_t pos = 0;
    while(pos < totalLen) {
        char* nl = memchr(buf + pos, '\n', totalLen - pos);
        char* lineStart = buf + pos;
        size_t lineLen = nl ? (size_t)(nl - lineStart) : (totalLen - pos);
        pos = nl ? (size_t)(nl - buf) + 1 : totalLen;

        if(lineLen > 0 && lineStart[lineLen - 1] == '\r') {
            lineLen--;
        }
        lineStart[lineLen] = '\0';

        if(is_hist_timestamp(lineStart)) {
            if(inTimestamped && hasEntryLines && entryLen > 0) {
                add_history(entryBuf);
                history_entry_set_timestamp(pendingTimestamp);
            }
            entryLen = 0;
            hasEntryLines = false;
            inTimestamped = true;
            pendingTimestamp = lineStart;
        } else if(inTimestamped) {
            if(!append_entry_line(&entryBuf, &entryLen, &entryCap, lineStart, lineLen, hasEntryLines)) {
                free(entryBuf);
                free(buf);
                return false;
            }
            hasEntryLines = true;
        } else {
            if(lineLen > 0) {
                add_history(lineStart);
            }
        }
    }

    if(inTimestamped && hasEntryLines && entryLen > 0) {
        add_history(entryBuf);
        history_entry_set_timestamp(pendingTimestamp);
    }

    free(entryBuf);
    free(buf);
    return true;
}

static bool write_all(int fd, const char* buf, size_t len)
{
    size_t written = 0;
    while(written < len) {
        ssize_t n = write(fd, buf + written, len - written);
        if(n < 0) {
            if(errno == EINTR) {
                continue;
            }
            return false;
        }
        written += (size_t)n;
    }
    return true;
}

static bool history_mgmt_write_history_file(const char* historyFile)
{
    if(is_zsh_parent_shell()) {
        return write_history(historyFile) == 0;
    }

    int fd = open(historyFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(fd < 0) {
        return false;
    }

    bool ok = true;
    HIST_ENTRY** list = history_list();
    if(list) {
        for(int i = 0; i < history_length; i++) {
            if(!list[i] || !list[i]->line) {
                continue;
            }
            if(list[i]->data) {
                const char* ts = (const char*)list[i]->data;
                if(!write_all(fd, ts, strlen(ts)) || !write_all(fd, "\n", 1)) {
                    ok = false;
                    break;
                }
            }
            if(!write_all(fd, list[i]->line, strlen(list[i]->line)) || !write_all(fd, "\n", 1)) {
                ok = false;
                break;
            }
        }
    }

    if(close(fd) != 0) {
        ok = false;
    }
    return ok;
}

char* parse_history_line(char *l)
{
#ifndef HSTR_TESTS_UNIT
    static bool isZsh, c;
    if(!c) {
        isZsh=is_zsh_parent_shell();
        c=true;
    }
#endif

    if(
#ifndef HSTR_TESTS_UNIT
    !isZsh ||
#endif
    !l ||
    l[0]!=':' ||
    l[1]!=' ') {
        return l;
    }

    // In zsh history file, the format of item CAN BE prefixed w/ timestamp
    // [:][blank][unix_timestamp][:][0][;][cmd]
    // Such as:
    // : 1420549651:0;ls /tmp/b
    // And the limit of unix timestamp 9999999999 is 2289/11/21,
    // so we could skip first 15 chars in every zsh history item to get the cmd.
    char *pt=l+2;
    unsigned u;
    for(u=0; u<ZSH_HISTORY_EXT_DIGITS && isdigit(*pt); u++, pt++);
    if (u==ZSH_HISTORY_EXT_DIGITS && *pt==':') {
        pt++;
        while(isdigit(*pt)) {
            pt++;
            if(*pt==';')
                return ++pt;
        }
    }

    return l;
}

// true if history line (w/ optional zsh extended history prefix) is the command cmd - zsh
// metafied characters are unmetafied before comparison
bool history_line_equals(char* line, const char* cmd, bool isZsh)
{
    char* command=parse_history_line(line);
    if(!command || !cmd) {
        return false;
    }
    if(isZsh && strchr(command, ZSH_META)) {
        char* unmetafied=hstr_strdup(command);
        bool equals=unmetafied && !strcmp(zsh_unmetafy(unmetafied, NULL), cmd);
        free(unmetafied);
        return equals;
    }
    return !strcmp(command, cmd);
}

bool history_mgmt_load_history_file(const char* historyFile)
{
    if(!historyFile) {
        fprintf(stderr, "\nUnable to read history file from ''!\n");
        return false;
    }
    if(!is_zsh_parent_shell()) {
        return load_bash_history_file(historyFile);
    }
    // TODO memleak in readline lib: read_history() > add_history()
    if(read_history(historyFile) != 0) {
        fprintf(stderr, "\nUnable to read history file from '%s'!\n", historyFile);
        return false;
    }
    return true;
}

HistoryItems* prioritized_history_create(int optionBigKeys, HashSet *blacklist)
{
    using_history();
    char* historyFile = get_history_file_name();
    bool loaded = history_mgmt_load_history_file(historyFile);
    free(historyFile);
    if(!loaded) {
        return NULL;
    }
    HISTORY_STATE* historyState=history_get_history_state();

    if(historyState->length > 0) {
        HashSet rankmap;
        hashset_init(&rankmap);

        RadixSorter rs;
        unsigned radixMaxKeyEstimate=historyState->size*1000;
        radixsort_init(&rs, (radixMaxKeyEstimate<100000?100000:radixMaxKeyEstimate));
        rs.optionBigKeys=optionBigKeys;

        RankedHistoryItem *r;
        RadixItem *radixItem;
        HIST_ENTRY **historyList=history_list();
        char **rawHistory=calloc(historyState->length, sizeof(char*));
        int rawOffset=historyState->length-1;
        bool isZsh=is_zsh_parent_shell();
        char **unmetafiedHistory=NULL;
        unsigned unmetafiedCount=0;
        char *line;
        int i;
        for(i=0; i<historyState->length; i++, rawOffset--) {
            if(!historyList[i] || !historyList[i]->line || !strlen(historyList[i]->line)) {
                continue;
            }

            if(is_hist_timestamp(historyList[i]->line)) {
                continue;
            }

            line=parse_history_line(historyList[i]->line);
            if(!line || !*line) {
                continue;
            }
            if(isZsh && strchr(line, ZSH_META)) {
                if(!unmetafiedHistory) {
                    unmetafiedHistory=malloc(sizeof(char*) * historyState->length);
                }
                char* unmetafied=unmetafiedHistory ? hstr_strdup(line) : NULL;
                if(unmetafied) {
                    line=zsh_unmetafy(unmetafied, NULL);
                    unmetafiedHistory[unmetafiedCount++]=line;
                }
            }
            rawHistory[rawOffset]=line;
            if(hashset_contains(blacklist, line)) {
                continue;
            }
            if((r=hashset_get(&rankmap, line))==NULL) {
                r=malloc(sizeof(RankedHistoryItem));
                r->rank=history_ranking_function(0, i, strlen(line));
                r->item=hstr_strdup(line);

                hashset_put(&rankmap, line, r);

                radixItem=malloc(sizeof(RadixItem));
                radixItem->key=r->rank;
                radixItem->data=r;
                radixItem->next=NULL;
                radixsort_add(&rs, radixItem);
            } else {
                radixItem=radix_cut(&rs, r->rank, r);

                assert(radixItem);

                if(radixItem) {
                    r->rank=history_ranking_function(r->rank, i, strlen(line));
                    radixItem->key=r->rank;
                    radixsort_add(&rs, radixItem);
                }
            }
        }

        // rankmap's keys and values have owners - just destroy the search structure
        hashset_destroy(&rankmap, false);

        // compact skipped entries while preserving newest-first order
        rawOffset=0;
        for(i=0; i<historyState->length; i++) {
            if(rawHistory[i]) {
                rawHistory[rawOffset++]=rawHistory[i];
            }
        }

        DEBUG_RADIXSORT();

        RadixItem** prioritizedRadix=radixsort_dump(&rs);
        prioritizedHistory=malloc(sizeof(HistoryItems));
        prioritizedHistory->count=rs.size;
        prioritizedHistory->rawCount=rawOffset;
        prioritizedHistory->items=malloc(rs.size * sizeof(char*));
        prioritizedHistory->rawItems=rawHistory;
        prioritizedHistory->unmetafiedItems=unmetafiedHistory;
        prioritizedHistory->unmetafiedCount=unmetafiedCount;
        unsigned u;
        for(u=0; u<rs.size; u++) {
            if(prioritizedRadix[u]->data) {
                char* item = ((RankedHistoryItem*)(prioritizedRadix[u]->data))->item;
                prioritizedHistory->items[u]=item;
            }
            free(prioritizedRadix[u]->data);
            free(prioritizedRadix[u]);
        }
        free(prioritizedRadix);

        radixsort_destroy(&rs);

        // history/readline cleanup, clear_history() called on exit as entries are used by raw view
        free(historyState);
        return prioritizedHistory;
    } else {
        // history/readline cleanup, clear_history() called on exit as entries are used by raw view
        printf("No history - nothing to suggest...\n");
        free(historyState);
        return NULL;
    }

}

void prioritized_history_destroy(HistoryItems* h)
{
    if(h) {
        if(h->items) {
            if(h->count) {
                unsigned i;
                for(i=0; i<h->count; i++) {
                    free(h->items[i]);
                }
            }
            free(h->items);
        }

        if(h->rawItems) {
            free(h->rawItems);
        }

        if(h->unmetafiedItems) {
            unsigned i;
            for(i=0; i<h->unmetafiedCount; i++) {
                free(h->unmetafiedItems[i]);
            }
            free(h->unmetafiedItems);
        }

        free(h);

        // readline/history cleanup
        free(history_get_history_state());
    }
    history_mgmt_clear_history();
}

void history_mgmt_open(void)
{
    dirty=false;
}

void history_mgmt_clear_dirty(void)
{
    dirty=false;
}

int history_mgmt_remove_from_system_history(char *cmd)
{
    if(!cmd) {
        return 0;
    }

    bool isZsh = is_zsh_parent_shell();
    int occurences = 0;
    HISTORY_STATE *historyState = history_get_history_state();
    // iterate backwards - removal shifts only entries which were already checked
    if(historyState) {
        for(int offset = historyState->length - 1; offset >= 0; offset--) {
            HIST_ENTRY **entries = history_list();
            if(!entries || !entries[offset] || !entries[offset]->line) {
                continue;
            }
            if(history_line_equals(entries[offset]->line, cmd, isZsh)) {
                occurences++;
                bool hasStandalonePrevTs = (offset > 0 && entries[offset - 1]
                    && !entries[offset - 1]->data && entries[offset - 1]->line
                    && is_hist_timestamp(entries[offset - 1]->line));
                HIST_ENTRY *removed = remove_history(offset);
                if(removed) {
                    free(free_history_entry(removed));
                }
                // remove standalone Bash timestamp entry if present in memory (e.g. unit tests)
                if(hasStandalonePrevTs) {
                    HIST_ENTRY *ts = remove_history(offset - 1);
                    if(ts) {
                        free(free_history_entry(ts));
                    }
                    offset--;
                }
            }
        }
    }
    free(historyState);

    if(occurences) {
        char *historyFile = get_history_file_name();
        history_mgmt_write_history_file(historyFile);
        free(historyFile);
        dirty = true;
    }
    return occurences;
}

bool history_mgmt_remove_last_history_entry(bool verbose)
{
    // the last command belongs to the current shell, therefore it is deleted
    // from the shell history file (HISTFILE) - HSTR_HISTFILE may be shared by MULTIPLE shells
    using_history();
    char *historyFile = get_shell_history_file_name();
    if(!history_mgmt_load_history_file(historyFile)) {
        free(historyFile);
        return false;
    }
    HISTORY_STATE *historyState=history_get_history_state();

    // delete the last command + the command that was used to run HSTR
    if(historyState->length > 1) {
        // length is NOT updated on history entry removal
        if(verbose) {
            fprintf(stdout, "Deleting command '%s' from history\n", historyState->entries[historyState->length-2]->line);
        }
        HIST_ENTRY *last = remove_history(historyState->length-1);
        if(last) {
            free(free_history_entry(last));
        }
        HIST_ENTRY *prev = remove_history(historyState->length-2);
        if(prev) {
            free(free_history_entry(prev));
        }
        bool ok = history_mgmt_write_history_file(historyFile);
        free(historyState);
        free(historyFile);
        history_mgmt_clear_history();

        return ok;
    }

    if(verbose) {
        fprintf(stderr, "Unable to delete the last command from history.\n");
    }

    free(historyState);
    free(historyFile);
    history_mgmt_clear_history();
    return false;
}

int history_mgmt_remove_from_raw(char *cmd, HistoryItems *history) {
    unsigned occurences=history->rawCount;
    if(history->rawCount) {
        unsigned i, ii;
        for(i=0, ii=0; i<history->rawCount; i++) {
            if(strcmp(cmd, history->rawItems[i])) {
                history->rawItems[ii++]=history->rawItems[i];
            }
        }
        history->rawCount=ii;
    }
    return occurences-history->rawCount;
}

int history_mgmt_remove_from_ranked(char *cmd, HistoryItems *history) {
    unsigned occurences=history->count;
    if(history->count) {
        unsigned i, ii;
        for(i=0, ii=0; i<history->count; i++) {
            if(strcmp(cmd, history->items[i])) {
                history->items[ii++]=history->items[i];
            }
        }
        history->count=ii;
    }
    return occurences-history->count;
}

void history_mgmt_flush(void)
{
    if(dirty && !is_zsh_parent_shell()) {
        fill_terminal_input("history -r\n", false);
    }
}
