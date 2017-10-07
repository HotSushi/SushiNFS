# SushiNFS

This is Simple [NFS](https://tools.ietf.org/html/rfc1094) implementation using [gRPC](https://github.com/grpc/) for RPC communication between server and client.

## Prerequisites 
1. [gRPC](https://github.com/grpc/grpc/blob/master/INSTALL.md) currently to install gRPC for C++, you need to build from source.
2. [Protocol Buffers v3](https://github.com/grpc/grpc/blob/master/INSTALL.md#build-from-source) will be installed automatically while building gRPC.

## Building
You need to set environment variable `GRPC_SOURCE` to point to the directory with grpc source built above.
```
user:~$ export GRPC_SOURCE=~/grpc/
```
Build the project by running `make` in the main directory. If something goes wrong try `make clean` and then `make`.
```
user:~/SushiNFS$ make
```

## Running
Both server and client executables will be present in `/bin` directory.
- To run server
  ```
  user:~/SushiNFS/bin$ ./HelloServer
  ```
- To run client
  ```
  user:~/SushiNFS/bin$ ./HelloClient
  ```
