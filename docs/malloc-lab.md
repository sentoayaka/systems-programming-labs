# Malloc Lab

中文 | [English](#english-version)

## 目标

实现一个动态内存分配器，提供 `malloc`、`free` 和 `realloc`，并在 trace 驱动测试中平衡空间利用率和吞吐量。

## 实现说明

源码：[code/malloc/mm.c](../code/malloc/mm.c)

- 使用 block header/footer 记录块大小和分配状态。
- 使用显式空闲链表管理可复用空闲块。
- 在放置时对过大的空闲块进行 split。
- 在释放后合并相邻空闲块，降低外部碎片。
- 对 `realloc` 做了原地扩展、拆分剩余空间和复制迁移等处理。
- 增加 heap consistency check，用于检查连续空闲块、块元数据和空闲链表一致性。

## 本地结果

本地目录中保留了多个 allocator 迭代版本，最终版本标记为 `93_score`。公开仓库保留核心源码，未包含课程 driver、trace 和二进制产物。

## 收获

这个项目让我真正理解了 allocator 里 metadata layout、alignment、fragmentation、coalescing、throughput/utilization tradeoff 的关系。最有价值的调试经验是尽早写 consistency checker，因为堆损坏通常不会在出错行附近立刻暴露。

## English Version

## Goal

Implement a dynamic memory allocator with `malloc`, `free`, and `realloc`, balancing space utilization and throughput under trace-driven tests.

## Implementation

Source: [code/malloc/mm.c](../code/malloc/mm.c)

- Used block headers and footers to track allocation state and block size.
- Maintained an explicit free list for reusable free blocks.
- Split oversized free blocks during placement.
- Coalesced adjacent free blocks after `free` to reduce external fragmentation.
- Handled `realloc` through in-place expansion, remainder splitting, and copy-based migration when needed.
- Added heap consistency checks for contiguous free blocks, metadata, and free-list invariants.

## Local Result

My local workspace keeps several allocator iterations, with the final version marked as `93_score`. The public repository keeps the core source file while excluding course drivers, traces, and binaries.

## Takeaway

This project made allocator metadata layout, alignment, fragmentation, coalescing, and throughput/utilization tradeoffs concrete. The most useful debugging habit was building a consistency checker early, because heap corruption often becomes visible far away from the line that caused it.
