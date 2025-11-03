# synapse

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.15%2B-green.svg)](https://cmake.org)

A high-performance Matrix server written in C++17, compliant with the Matrix specification.

## Features

- ✅ Full support for the Matrix Client-Server API
- 🔒 End-to-end encryption (Olm, Megolm)
- 🌐 Federation support
- 🚀 High performance
- 🐳 Docker containerization
- 📊 Monitoring and metrics
- 🧪 Full test coverage

## Requirements

- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.15+
- Conan 2.0+
- PostgreSQL 12+ or SQLite3
- Redis (optional, for caching)

# Start

### Build from sources

```
https://github.com/The-IDK-Project/synapse.git
cd synapse
conan install . --build missing
cmake --preset default
cmake --build build
mkdir build && cd build
cmake ..
make -j$(nproc)
```
### Start from Docker

```
docker-compose up -d

docker build -t matrix-server .
docker run -p 8008:8008 -p 8448:8448 matrix-server
```

# Configuration

### Copy the example configuration:
````
cp config/config.yaml.example config/config.yaml
cp config/log_config.yaml.example config/log_config.yaml
````
Configure the database and other settings in config/config.yaml

### Start the server:
````
./build/src/matrix-server config/config.yaml
````
API Documentation

The server provides the following API endpoints:

Client-Server API: /_matrix/client/r0
Server-Server API: /_matrix/federation/v1
WebSocket API: /ws for realtime

Detailed documentation is available in .
# Architecture

The project is organized into the following modules:
```
core/ - basic Matrix types and logic
api/ - HTTP and WebSocket API
storage/ - database and cache management
crypto/ - cryptographic operations
network/ - network infrastructure
utils/ - auxiliary utilities
```
More details in docs/architecture.md.
Development
Project Structure

```
matrix-server-cpp/
├── include/ # Header Files
├── src/ # Source Code
├── tests/ # Unit and Integration Tests
├── tools/ # Helper Utilities
├── benchmarks/ # Performance Benchmarks
└── docs/ # Documentation
```
Development Build
```
cmake --preset debug
cmake --build build-debug
cd build-debug && ctest --verbose
```

```
cmake --preset coverage
cmake --build build-coverage
cd build-coverage && make coverage
```
# Code Formatting

### Format Everything Code
find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i

### Style check
find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-tidy

# Contribute

Fork the repository

Create a feature branch ```git checkout -b The-IDK-Project/synapse```

Commit the changes ```git commit -m 'Add 30 handler file'```

Push the branch ```git push origin master/synapse```

Open a Pull Request

# License

This project is licensed under the Apache License 2.0 - see LICENSE for details.
Support

# URL
Matrix rooms: #theidkteam:matrix.org

Issues: https://github.com/The-IDK-Project/synapse/issues

Documentation: docs/