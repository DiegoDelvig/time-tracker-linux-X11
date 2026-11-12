# time-tracker-linux-X11

A small Linux/X11 utility that tracks how much time you spend on each application by monitoring the currently active window and aggregating usage by process name.

## Overview

This project reads the active X11 window via X11 properties, resolves its PID, and then looks up the process name from `/proc/<pid>/comm`. It accumulates the total seconds spent on each application and prints the results.

It is implemented in C and built with a simple `Makefile`.

## Features

- Monitors the active X11 window
- Resolves the window's PID
- Reads the executable name from `/proc`
- Aggregates time per application
- Simple build/test workflow with Make
- Includes a unit test for the statistics module

## Project structure

- `src/` — core implementation
  - `main.c` — example runner
  - `tracker.c` / `tracker.h` — tracker orchestration
  - `x11_probe.c` / `x11_probe.h` — X11 active-window probing
  - `proc_info.c` / `proc_info.h` — process-name lookup
  - `stats.c` / `stats.h` — aggregate statistics container
  - `ticker.c` / `ticker.h` — periodic tick callback helper
- `tests/test_stats.c` — basic validation for the stats module
- `Makefile` — build and test targets

## Requirements

- Linux
- X11 display server
- GCC or Clang
- Make
- Optional: Valgrind for running the test target

## Build

```bash
make
```

This produces the binary in `build/tracker`.

## Run

```bash
./build/tracker
```

The current implementation runs a sample loop and prints a list of applications and accumulated seconds.

## Test

```bash
make test
```

The test target builds and runs the stats validation program using Valgrind.

## Example output

```text
nom : firefox -> 8
nom : bash -> 2
```

## Notes

- This project depends on X11 and is intended for Linux/X11 desktop environments.
- The logic treats the active application's process name as a key, so usage is grouped by app name rather than by window title.
- The code currently focuses on a lightweight, dependency-minimal implementation rather than a full UI.
