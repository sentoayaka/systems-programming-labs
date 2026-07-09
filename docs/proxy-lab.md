# Proxy Lab

中文 | [English](#english-version)

## 目标

实现一个 HTTP proxy，能够接收客户端请求、转发到服务器、返回响应，并缓存可复用对象。

## 实现说明

源码：

- [code/proxy/proxy.c](../code/proxy/proxy.c)
- [code/proxy/cache.c](../code/proxy/cache.c)
- [code/proxy/cache.h](../code/proxy/cache.h)

实现内容：

- 解析 HTTP request line 和 headers。
- 将请求改写为适合 proxy 转发的格式。
- 使用 robust I/O wrappers 处理 socket 读写。
- 支持并发处理多个客户端连接。
- 实现对象缓存模块，保存缓存对象及其替换相关元数据。

## 收获

这个项目把 socket、HTTP 解析、并发、同步和缓存策略放在同一个系统里，是从底层 C 代码走向可交互网络服务的一次很好的练习。

## English Version

## Goal

Build an HTTP proxy that accepts client requests, forwards them to end servers, returns responses, and caches reusable web objects.

## Implementation

Source:

- [code/proxy/proxy.c](../code/proxy/proxy.c)
- [code/proxy/cache.c](../code/proxy/cache.c)
- [code/proxy/cache.h](../code/proxy/cache.h)

What I implemented:

- Parsed HTTP request lines and headers.
- Rewrote requests into a proxy-friendly forwarding format.
- Used robust I/O wrappers for socket reads and writes.
- Supported concurrent handling of multiple client connections.
- Implemented an object-cache module with replacement-related metadata.

## Takeaway

This project connected sockets, HTTP parsing, concurrency, synchronization, and caching in one system. It was a useful bridge from low-level C programming to an interactive network service.
