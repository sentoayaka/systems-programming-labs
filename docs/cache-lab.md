# Cache Lab

## Goal

Build a cache simulator and optimize matrix transpose routines for a direct-mapped cache model.

## Design Summary

- Parsed memory traces and simulated cache hits, misses, and evictions.
- Represented cache sets and lines explicitly, including valid bits, tags, and recency state.
- Optimized matrix transpose with blocking and temporary-register strategies.
- Treated `32x32`, `64x64`, and irregular matrix shapes differently because they stress cache conflicts in different ways.

## Local Result

The local cache simulator result snapshot is:

```text
3473 4724 4692
```

## What I Learned

The main lesson was that asymptotic complexity is not enough for performance-sensitive code. Memory access order and conflict patterns can dominate runtime, especially under direct mapping and small cache sizes.
