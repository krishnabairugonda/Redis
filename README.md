# Redis-like Key-Value Server in C++

A lightweight Redis-inspired in-memory key-value server implemented from scratch in **C++** using low-level Linux networking and socket APIs.

The project explores how a basic Redis-style server can be built from the ground up, including TCP communication, request/response protocols, key-value storage, and handling multiple clients.

## Features

- TCP client-server communication
- Custom request/response protocol
- In-memory key-value storage
- SET and GET style operations
- Multiple client connections
- Concurrent client testing
- Non-blocking socket experiments
- Event-driven server experiments using `poll()`
- C++ implementation using Linux socket APIs
- Separate client and server implementations

## Project Structure

```text
.
├── client/
│   ├── client_2.cpp
│   ├── concc_test_client.cpp
│   ├── echo_tcp_client.cpp
│   ├── key_value_client.cpp
│   ├── kv_client
│   ├── output/
│   │   ├── concc_client
│   │   ├── echo_tcp_client
│   │   └── kv_client
│   └── req_resp_client.cpp
│
├── server/
│   ├── concc_server.cpp
│   ├── echo_tcp_server.cpp
│   ├── key_value_server.cpp
│   ├── output/
│   │   ├── echo_tcp_server
│   │   ├── kv_server
│   │   └── server
│   └── req_resp_server.cpp
│
└── git_cheat-sheet.pdf
