# Proxy Lab

## Goal

Build an HTTP proxy that accepts client requests, forwards them to end servers, returns responses, and caches reusable web objects.

## Design Summary

- Parsed HTTP request lines and headers.
- Rewrote requests into a proxy-friendly format for forwarding.
- Used robust I/O wrappers for socket reads and writes.
- Added concurrency so multiple clients could be served at once.
- Added a cache module for reusable objects, with metadata for replacement decisions.

## What I Learned

The proxy lab connected several systems topics at once: sockets, parsing, concurrency, synchronization, and caching. It was a useful bridge from low-level C mechanics to a user-visible network service.
