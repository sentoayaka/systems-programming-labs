# Malloc Lab

## Goal

Implement a dynamic memory allocator with `malloc`, `free`, and `realloc` under CS:APP-style heap and trace constraints.

## Design Summary

- Used block headers and footers to track allocation state and block size.
- Maintained an explicit free list for reusable free blocks.
- Split oversized free blocks during placement when the remainder was large enough to stay useful.
- Coalesced adjacent free blocks to reduce fragmentation.
- Added heap consistency checks for contiguous free blocks, allocation metadata, and free-list correctness.
- Iterated on several allocator variants before keeping a final higher-scoring version locally.

## What I Learned

The allocator forced me to think about metadata layout, alignment, fragmentation, and the tension between utilization and throughput. The most useful debugging habit was building consistency checks early, because allocator bugs often appear far from the line that created the corrupted block.

## Portfolio Note

The complete allocator source is kept private because this is a course lab. The public artifact documents the design and validation strategy.
