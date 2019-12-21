/*
 radixsort.c        radix sort

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "include/radixsort.h"

#define GET_TOP_INDEX(KEY) KEY/RADIX_SLOT_SIZE
#define GET_LOW_INDEX(KEY) KEY%RADIX_SLOT_SIZE

void radixsort_init(RadixSorter* rs, unsigned keyLimit)
{
    rs->optionBigKeys=RADIX_BIG_KEYS_SKIP;

    rs->_topIndexLimit=GET_TOP_INDEX(keyLimit);
    rs->size=0;
    rs->topDigits=malloc(rs->_topIndexLimit * sizeof(RadixItem***));
    memset(rs->topDigits, 0, rs->_topIndexLimit * sizeof(RadixItem***));
    rs->maxKey=0;
    rs->keyLimit=keyLimit;

    rs->_slotDescriptors=malloc(rs->_topIndexLimit * sizeof(RadixSlot**));
    rs->_slotsCount=0;
    rs->_debug=RADIX_DEBUG_LEVEL_NONE;
}

void radixsort_destroy(RadixSorter* rs)
{
    // radix items: DONE (passed on dump() by reference)
    // rs: DONE (created and destroyed by caller)
    // slots:
    int topIndex = GET_TOP_INDEX(rs->maxKey);
    do {
        if(rs->topDigits[topIndex]) {
            free(rs->topDigits[topIndex]);
            free(rs->_slotDescriptors[topIndex]);
        }
    } while(--topIndex>=0);
    free(rs->topDigits);
    free(rs->_slotDescriptors);
}

void radixsort_set_debug_level(RadixSorter* rs, unsigned debugLevel)
{
    rs->_debug=debugLevel;
}

RadixItem** radixsort_get_slot(RadixSorter* rs, unsigned topIndex)
{
    RadixItem **slot=malloc(RADIX_SLOT_SIZE * sizeof(RadixItem*));
    memset(slot, 0, RADIX_SLOT_SIZE * sizeof(RadixItem*));

    RadixSlot *descriptor=malloc(sizeof(RadixSlot));
    descriptor->min=rs->keyLimit;
    descriptor->max=0;
    descriptor->size=0;

    rs->_slotDescriptors[topIndex]=descriptor;
    rs->_slotsCount++;
    return slot;
}

void radixsort_add(RadixSorter* rs, RadixItem* item)
{
    if(item->key > rs->keyLimit) {
        if(rs->_debug > RADIX_DEBUG_LEVEL_NONE) {
            fprintf(stderr, "WARNING: Radix sort overflow - inserted key is bigger than limit (%u): %u\n", rs->keyLimit, item->key);
        }
        if(rs->optionBigKeys==RADIX_BIG_KEYS_FLOOR) {
            item->key = rs->keyLimit-1;
        } else {
            if(rs->optionBigKeys==RADIX_BIG_KEYS_SKIP) {
                return;
            } else {
                exit(0);
            }
        }
    }

    unsigned topIndex = GET_TOP_INDEX(item->key);
    unsigned lowIndex = GET_LOW_INDEX(item->key);

    if(!rs->topDigits[topIndex]) {
        rs->topDigits[topIndex]=radixsort_get_slot(rs, topIndex);
    }

    RadixItem* chain=rs->topDigits[topIndex][lowIndex];
    rs->topDigits[topIndex][lowIndex]=item;
    if(chain==NULL) {
        item->next=NULL;
    } else {
        item->next=chain;
    }

    rs->size++;
    rs->maxKey=MAX(rs->maxKey,item->key);
    rs->_slotDescriptors[topIndex]->min=MIN(rs->_slotDescriptors[topIndex]->min,item->key);
    rs->_slotDescriptors[topIndex]->max=MAX(rs->_slotDescriptors[topIndex]->max,item->key);
    rs->_slotDescriptors[topIndex]->size++;
}

void radix_dec_slot_descriptor_size(RadixSorter* rs, RadixSlot *descriptor, unsigned key, unsigned topIndex)
{
    UNUSED_ARG(key);

    descriptor->size--;
    if(!descriptor->size) {
        descriptor->min=rs->keyLimit;
        descriptor->max=0;
    } else {
        if(descriptor->size==1) {
            if(rs->topDigits[topIndex][GET_LOW_INDEX(descriptor->max)]) {
                descriptor->min=descriptor->max;
            } else {
                if(rs->topDigits[topIndex][GET_LOW_INDEX(descriptor->min)]) {
                    descriptor->max=descriptor->min;
                }
            }
        }
    }
}

RadixItem* radix_cut(RadixSorter* rs, unsigned key, void* data)
{
    // TODO optimization: fix min/max on cut of a value
    if(key <= rs->maxKey) {
        unsigned topIndex = GET_TOP_INDEX(key);
        unsigned lowIndex = GET_LOW_INDEX(key);

        if(rs->topDigits[topIndex]) {
            RadixItem *ri=rs->topDigits[topIndex][lowIndex];
            RadixItem *lastRi=NULL;
            while(ri->data!=data) {
                if(ri->next) {
                    lastRi=ri;
                    ri=ri->next;
                } else {
                    break;
                }
            }
            if(ri->data==data) {
                if(lastRi) {
                    lastRi->next=ri->next;
                } else {
                    rs->topDigits[topIndex][lowIndex]=ri->next;
                }
                ri->next=NULL;
                rs->size--;
                radix_dec_slot_descriptor_size(rs, rs->_slotDescriptors[topIndex], key, topIndex);
                return ri;
            }
        }
    }
    return NULL;
}

RadixItem** radixsort_dump(RadixSorter* rs)
{
    if(rs->size>0) {
        RadixItem **result=malloc(rs->size * sizeof(RadixItem *));
        int t = GET_TOP_INDEX(rs->maxKey);
        int slotMin, slotSize, slotCount, l;
        unsigned items=0;
        do {
            if(rs->topDigits[t]) {
                if(rs->_slotDescriptors[t]->size>0) {
                    l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max);
                    slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min);
                    slotSize=rs->_slotDescriptors[t]->size;
                    slotCount=0;
                    do {
                        if(rs->topDigits[t][l]) {
                            result[items++]=rs->topDigits[t][l];
                            slotCount++;
                            RadixItem *ri=rs->topDigits[t][l]->next;
                            while(ri) {
                                result[items++]=ri;
                                slotCount++;
                                ri=ri->next;
                            }
                        }
                    } while(--l>=slotMin && slotCount<slotSize);
                }
            }
        } while(--t>=0);
        return result;
    }
    return NULL;
}

void radixsort_stat(RadixSorter* rs, bool listing)
{
    printf("\n Radixsort (size/max/limit/slot count): %u %u %u %u", rs->size, rs->maxKey, rs->keyLimit, rs->_slotsCount);
    unsigned memory=rs->_topIndexLimit * sizeof(RadixItem ***);
    memory+=memory;
    memory+=rs->_slotsCount*(RADIX_SLOT_SIZE * sizeof(RadixItem *));
    printf("\n   Memory: %u\n", memory);
    if(listing && rs->size>0) {
        int t = GET_TOP_INDEX(rs->maxKey);
        int slotMin, slotSize, slotCount, l;
        unsigned items=0;
        do {
            if(rs->topDigits[t]) {
                printf("\n  Slot %u (size/min/max): %u %u %u",t, rs->_slotDescriptors[t]->size, rs->_slotDescriptors[t]->min, rs->_slotDescriptors[t]->max);
                if(rs->_slotDescriptors[t]->size>0) {
                    l=GET_LOW_INDEX(rs->_slotDescriptors[t]->max);
                    slotMin=GET_LOW_INDEX(rs->_slotDescriptors[t]->min);
                    slotSize=rs->_slotDescriptors[t]->size;
                    slotCount=0;
                    do {
                        if(rs->topDigits[t][l]) {
                            printf("\n    > %d #%u",l, ++items);
                            ++slotCount;
                            RadixItem *ri=rs->topDigits[t][l]->next;
                            while(ri) {
                                printf(" #%u",++items);
                                ++slotCount;
                                ri=ri->next;
                            }
                        }
                    } while(--l>=slotMin && slotCount<slotSize);
                    printf("\n    STOP @ %d",l);
                } else {
                    printf(" > SKIPPED");
                }
            }
        } while(--t>=0);
    }
    fflush(stdout);
}
