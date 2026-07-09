# Cache Lab

中文 | [English](#english-version)

## 目标

实现 cache simulator，并针对直接映射 cache 模型优化矩阵转置程序。

## 实现说明

源码：

- [code/cache/csim.c](../code/cache/csim.c)
- [code/cache/trans.c](../code/cache/trans.c)

实现内容：

- 解析 memory trace，模拟 hit、miss 和 eviction。
- 显式表示 cache set、line、valid bit、tag 和替换状态。
- 针对矩阵转置设计 blocking 与临时变量策略。
- 分别处理 `32x32`、`64x64` 和不规则矩阵，因为它们的 cache conflict 模式不同。

## 本地结果

本地 cache simulator 输出快照：

```text
3473 4724 4692
```

## 收获

这个项目说明了性能优化不能只看渐进复杂度。访问顺序、cache conflict 和数据布局会直接影响运行表现，尤其在小 cache 和 direct-mapped cache 下非常明显。

## English Version

## Goal

Build a cache simulator and optimize matrix transpose routines for a direct-mapped cache model.

## Implementation

Source:

- [code/cache/csim.c](../code/cache/csim.c)
- [code/cache/trans.c](../code/cache/trans.c)

What I implemented:

- Parsed memory traces and simulated hits, misses, and evictions.
- Represented cache sets, lines, valid bits, tags, and replacement state explicitly.
- Optimized matrix transpose with blocking and temporary-variable strategies.
- Treated `32x32`, `64x64`, and irregular matrices separately because they create different cache-conflict patterns.

## Local Result

Local cache simulator result snapshot:

```text
3473 4724 4692
```

## Takeaway

This project showed that performance optimization is not only about asymptotic complexity. Access order, cache conflicts, and data layout can dominate runtime, especially under small direct-mapped caches.
