# Systems Programming Labs

中文 | [English](#english-version)

## 项目简介

这是我学习 CMU 15-213 / CS:APP 过程中整理的系统编程作品集。对应的本地学习内容覆盖数据表示、汇编、cache 行为、动态内存分配、Unix shell job control 和 HTTP proxy。

这个公开仓库包含两部分：`docs/` 中的中英双语项目说明，以及 `code/` 中我实现的核心源码。为了保持仓库干净，我没有搬运整套课程 handout、PDF、二进制程序、trace 数据和编译产物。

## 项目覆盖

- [Malloc Lab](docs/malloc-lab.md)：显式空闲链表 allocator，源码见 [code/malloc/mm.c](code/malloc/mm.c)。
- [Cache Lab](docs/cache-lab.md)：cache simulator 与矩阵转置 cache 优化，源码见 [code/cache](code/cache)。
- [Shell Lab](docs/shell-lab.md)：支持前后台任务、信号转发和 job 状态迁移的 Unix shell，源码见 [code/shell/tsh.c](code/shell/tsh.c)。
- [Proxy Lab](docs/proxy-lab.md)：HTTP proxy，请求解析、并发处理和对象缓存，源码见 [code/proxy](code/proxy)。

## 能力展示

| 方向 | 证据 |
| --- | --- |
| C 系统编程 | 指针运算、位级布局、内存块、header/footer |
| 性能优化 | cache miss 降低、allocator utilization/throughput 权衡 |
| Unix 进程控制 | fork/exec、process group、SIGINT/SIGTSTP/SIGCHLD |
| 网络编程 | socket、HTTP request forwarding、robust I/O |
| 调试方法 | trace-based testing、reference output comparison、consistency checks |

## 本地结果快照

- Cache simulator 本地输出记录为 `3473 4724 4692`。
- Shell lab 使用 trace driver 与 reference output 对比验证。
- Malloc lab 本地保留了多个 allocator 版本，最终版本标记为 `93_score`。
- Proxy lab 本地实现拆分了 proxy 与 cache 模块。

## 为什么放在 GitHub 主页

这个项目展示我在系统方向的训练：能阅读低层规格、调试 C 程序、理解性能瓶颈，并用 trace 和 reference output 验证行为，而不是只靠临时样例。

## 学术诚信说明

本仓库公开我实现的核心源码和项目说明，但不镜像原始课程 handout、PDF、二进制程序、trace 数据和编译产物。

## English Version

This repository is a systems-programming portfolio based on my CMU 15-213 / CS:APP study work. My local work covered data representation, assembly, cache behavior, dynamic memory allocation, Unix shell job control, and HTTP proxy programming.

The public repository contains two parts: bilingual project reports under `docs/` and my core source files under `code/`. To keep the repository clean, I did not mirror full course handouts, PDFs, binaries, trace data, or build artifacts.

## Covered Projects

- [Malloc Lab](docs/malloc-lab.md): explicit free-list allocator; source: [code/malloc/mm.c](code/malloc/mm.c).
- [Cache Lab](docs/cache-lab.md): cache simulator and matrix-transpose cache optimization; source: [code/cache](code/cache).
- [Shell Lab](docs/shell-lab.md): Unix shell with foreground/background jobs, signal forwarding, and job-state transitions; source: [code/shell/tsh.c](code/shell/tsh.c).
- [Proxy Lab](docs/proxy-lab.md): HTTP proxy with request parsing, concurrency, and object caching; source: [code/proxy](code/proxy).

## Skills Demonstrated

| Area | Evidence |
| --- | --- |
| C systems programming | pointer arithmetic, bit-level layout, memory blocks, headers/footers |
| Performance engineering | cache miss reduction, allocator utilization/throughput tradeoffs |
| Unix process control | fork/exec, process groups, SIGINT/SIGTSTP/SIGCHLD handling |
| Networking | sockets, HTTP request forwarding, robust I/O wrappers |
| Debugging discipline | trace-based testing, reference-output comparison, consistency checks |

## Portfolio Summary

This work demonstrates the systems side of my preparation: I am comfortable reading low-level specifications, debugging C programs, reasoning about performance, and validating behavior with traces rather than only with ad-hoc examples.

## Academic Integrity

This repository publishes my core source files and project reports, while excluding original course handouts, PDFs, binaries, trace data, and build artifacts.
