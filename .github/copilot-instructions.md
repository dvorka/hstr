# Copilot instructions

This document provides instructions to the copilot AI assistants and agents.

## General instructions

- Write beautiful code.
- Handle for errors, and exceptions and corner cases.
- Do good and don't be evil.

## Functional architecture instructions

- This repository is a repo of a TUI based application for the completion from the Bash or Zsh history.

## Technology stack instructions

- Frontend: ncurses.
- Backend: C.

## Backend code instructions

- Application backend is written in C.
- Always start comments you generate with lowercase letter.

## Git instructions

- NEVER commit, amend, rebase, merge or push - the maintainer does all Git commits and pushes.
- Leave changes uncommitted in the working tree (staging is fine) for the maintainer to review.

## Security instructions

- ALWAYS follow security best practices.
- NEVER log form data that may contain sensitive information.
- ALWAYS consider overflow and memory corruption issues when writing C code.

## Backend code quality instructions

- NEVER use hacks or workarounds to make tests pass - always write clean, production-quality code.
- NEVER modify production code with ugly hacks just to make tests work - fix tests properly or remove them.
- ALWAYS follow clean code principles.

## Frontend instructions

- Application frontend is written in NCurses.

## Test instructions

- Test code is stored in the `tests/src` directory.
- Use `valgrind` to check for memory leaks and memory corruption issues after code changes.
- Tests can be run with `cd build && make test`.
- Write backend tests for all bugs being fixed.
- Use https://github.com/ThrowTheSwitch/Unity framework for testing.
- Each test (function) is structured into 3 sections: // GIVEN, // WHEN, and // THEN. // GIVEN section prepares the data, // WHEN section performs the actual test, and // THEN section asserts, checks and prints the results.
- Use `DONE` instead of emoji character ✓ (do not use emoji/unicode characters, use the text inside)

## Build instructions

- The project is built with `automake` and `make`.
- You can use `cd build && make build` to build the project.

## Documentation instructions

- ALWAYS document new features in the man page located in the `man/` directory.
