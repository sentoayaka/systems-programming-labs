# Shell Lab

## Goal

Implement a small Unix shell with job control, foreground/background execution, built-in commands, and signal handling.

## Design Summary

- Parsed command lines and detected background jobs.
- Implemented built-ins such as `quit`, `jobs`, `bg`, and `fg`.
- Used `fork`, `exec`, and process groups to run child jobs correctly.
- Forwarded `SIGINT` and `SIGTSTP` to the foreground process group.
- Reaped children and updated job states on `SIGCHLD`.
- Validated behavior with the provided trace driver and reference-output comparison.

## What I Learned

This lab made signal races concrete. The important engineering detail was to carefully order signal blocking, job-list mutation, child creation, and waiting logic so that state transitions stayed consistent.
