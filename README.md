# cache-proxy-server

Cache Proxy Server Implementation based on C++

---

The project has been configured and built in Docker.
A user can configure her Internet browser to point at the server running this project. 
This cache proxy will then forward requests to and from the website they are trying to reach and their internet browser.

## Functionality

---

1. Your proxy should support required methods
   1. Support GET
   2. Support POST
   3. Support CONNECT

2. Your proxy should cache appropriate responses
   1. Rule of Expiration Time
   2. Rule of Re-validation
   3. Cache Policy

3. Concurrency
   1. Spawn a thread/process to handle a request
   2. Handle necessary synchronization for cache.

4. Support Log files.

5. Robust to external failures.

6. Exception Guarantees.