# Scrolling regression tests

Build HSTR normally, then run the ncurses integration tests with Python 3 and
[pyte](https://pypi.org/project/pyte/) installed:

```sh
python3 test/src/test_scroll.py "$PWD/src/hstr"
```

The tests open pseudo-terminals and use temporary home directories and history
files. They do not read or change the user's history. No running shell integration
or TIOCSTI support is needed.

Coverage includes scrolling beyond the first page, Page Up/Down, both list ends,
resizing, deleting after scrolling, preserving the selected screen row after
repeated deletion and deletion on the final page, query changes, regular
expressions, empty results, and all prompt/help placements.
Timestamp-delimited Bash multiline entries are checked for one-row rendering,
scrolling, paging, resizing, regex and substring filtering, ranked view, exact
selection output, deletion confirmation, and preservation after deletion and
reopening the history file.

These are terminal integration tests; the existing Unity backend tests can be
run separately as documented in the build scripts.
