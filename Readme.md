
### Simple C++ Server (TCP + protobuf) counting unique words in data sent by clients.

- Based on boost asio tcp.
- Processing of incoming text data is done with boost threadpool.
- Parsed text data is stored to TBB concurrent unordered set.

## Implementation details 
- Processing of incoming Request message starts in `session::processRequest`

## Compiling and running C++ Server

### Building the server

```
mkdir build
cd build
cmake ..
make
```

Proto source files are generated into the `./proto` directory

### Running with default port 8080 and binding to all interfaces
```
./server
```

### Running with custom port

```
./server <port>
```

### Running with custom address and port
```
./server <address> <port>
```