# Shell Lab

中文 | [English](#english-version)

## 目标

实现一个小型 Unix shell，支持前台/后台任务、内置命令、job control 和信号处理。

## 实现说明

源码：[code/shell/tsh.c](../code/shell/tsh.c)

- 解析命令行，识别前台和后台任务。
- 实现 `quit`、`jobs`、`bg`、`fg` 等内置命令。
- 使用 `fork`、`exec` 和 process group 正确运行子进程。
- 将 `SIGINT` 和 `SIGTSTP` 转发给前台进程组。
- 处理 `SIGCHLD`，回收子进程并更新 job 状态。
- 使用 trace driver 与 reference output 对比验证行为。

## 收获

这个项目让我真正理解了信号竞争和 job list 状态维护。关键工程点是安排好 signal blocking、fork、job-list mutation 和 wait 逻辑的顺序，否则 shell 很容易出现偶发错误。

## English Version

## Goal

Implement a small Unix shell with foreground/background jobs, built-in commands, job control, and signal handling.

## Implementation

Source: [code/shell/tsh.c](../code/shell/tsh.c)

- Parsed command lines and detected foreground/background jobs.
- Implemented built-ins such as `quit`, `jobs`, `bg`, and `fg`.
- Used `fork`, `exec`, and process groups to run child processes correctly.
- Forwarded `SIGINT` and `SIGTSTP` to the foreground process group.
- Handled `SIGCHLD` to reap children and update job states.
- Validated behavior with trace-driver and reference-output comparisons.

## Takeaway

This project made signal races and job-list state management concrete. The key engineering detail was ordering signal blocking, fork, job-list mutation, and wait logic carefully; otherwise the shell can fail intermittently.
