# CSAPP Systems Programming Portfolio

Portfolio notes for my CMU 15-213 / CS:APP systems programming study. The local work covered data representation, assembly, cache behavior, dynamic memory allocation, shell job control, and network proxy programming.

This repository is intentionally a public-facing summary instead of a full lab-solution dump. It explains the design choices, testing strategy, and systems concepts demonstrated by the labs while respecting course material and academic-integrity boundaries.

## Projects Covered

- [Malloc Lab](docs/malloc-lab.md): explicit free-list allocator with splitting, coalescing, realloc handling, and heap consistency checks.
- [Cache Lab](docs/cache-lab.md): cache simulator and matrix-transpose cache optimization.
- [Shell Lab](docs/shell-lab.md): Unix shell with foreground/background jobs, signal forwarding, and job-state transitions.
- [Proxy Lab](docs/proxy-lab.md): HTTP proxy with request parsing, concurrency, and object caching.

## Skills Demonstrated

| Area | Evidence |
| --- | --- |
| C systems programming | pointer arithmetic, bit-level layout, memory blocks, headers/footers |
| Performance engineering | cache miss reduction, allocator utilization/throughput tradeoffs |
| Unix process control | fork/exec, process groups, SIGINT/SIGTSTP/SIGCHLD handling |
| Networking | sockets, HTTP request forwarding, robust I/O wrappers |
| Debugging discipline | trace-based testing, reference-output comparison, consistency checks |

## Local Results Snapshot

- Cache simulator output recorded as `3473 4724 4692` in the local `.csim_results` file.
- Shell traces were run against the local shell implementation and compared with the reference driver output.
- Malloc work includes several allocator variants and a final `93_score` version in the local private workspace.
- Proxy work includes separate cache and proxy modules in the local private workspace.

## Why This Is On My Profile

This work demonstrates the systems side of my preparation: I am comfortable reading low-level specifications, debugging C programs, reasoning about performance, and validating behavior with traces rather than only with ad-hoc examples.

## Academic Integrity

The original course handouts and full solution files are not mirrored here. This repository is a curated portfolio report: it summarizes my implementation approach and learning outcomes without publishing complete answers for active course labs.
