#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"
CONFIG_FILE="${CONFIG_FILE:-$PROJECT_ROOT/config/config.yaml}"
LOG_FILE="${LOG_FILE:-$PROJECT_ROOT/logs/matrix-server.log}"
PID_FILE="${PID_FILE:-$PROJECT_ROOT/matrix-server.pid}"

check_dependencies() {
    if [ ! -f "$BUILD_DIR/src/matrix-server" ]; then
        echo "Error: Server binary not found at $BUILD_DIR/src/matrix-server"
        echo "Please build the server first: cmake --build build"
        exit 1
    fi

    if [ ! -f "$CONFIG_FILE" ]; then
        echo "Error: Config file not found at $CONFIG_FILE"
        echo "Please create config file from config.yaml.example"
        exit 1
    fi
}

setup_directories() {
    mkdir -p "$(dirname "$LOG_FILE")"
    mkdir -p "$PROJECT_ROOT/data"
    mkdir -p "$PROJECT_ROOT/keys"
}

check_ports() {
    for port in 8008 8448; do
        if lsof -Pi ":$port" -sTCP:LISTEN -t >/dev/null; then
            echo "Error: Port $port is already in use"
            exit 1
        fi
    done
}

start_server() {
    echo "Starting Matrix Server..."

    "$BUILD_DIR/src/matrix-server" "$CONFIG_FILE" >> "$LOG_FILE" 2>&1 &
    SERVER_PID=$!

    echo $SERVER_PID > "$PID_FILE"
    echo "Server started with PID: $SERVER_PID"

    sleep 2

    if kill -0 $SERVER_PID > /dev/null 2>&1; then
        echo "Server is running"
        echo "Logs: $LOG_FILE"
        echo "PID: $SERVER_PID (stored in $PID_FILE)"
    else
        echo "Error: Server failed to start. Check logs: $LOG_FILE"
        exit 1
    fi
}

wait_for_health() {
    echo "Waiting for server to become healthy..."
    local max_attempts=30
    local attempt=1

    while [ $attempt -le $max_attempts ]; do
        if curl -f http://localhost:8008/_matrix/client/versions > /dev/null 2>&1; then
            echo "Server is healthy and responding"
            return 0
        fi

        echo "Attempt $attempt/$max_attempts: Server not ready yet..."
        sleep 2
        attempt=$((attempt + 1))
    done

    echo "Error: Server failed to become healthy within timeout"
    exit 1
}

main() {
    case "${1:-start}" in
        "start")
            check_dependencies
            setup_directories
            check_ports
            start_server
            wait_for_health
            ;;
        "foreground")
            check_dependencies
            setup_directories
            check_ports
            echo "Starting Matrix Server in foreground..."
            exec "$BUILD_DIR/src/matrix-server" "$CONFIG_FILE"
            ;;
        *)
            echo "Usage: $0 {start|foreground}"
            exit 1
            ;;
    esac
}

main "$@"