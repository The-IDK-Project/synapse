#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PID_FILE="${PID_FILE:-$PROJECT_ROOT/matrix-server.pid}"

get_pid() {
    if [ -f "$PID_FILE" ]; then
        cat "$PID_FILE"
    else
        pgrep -f "matrix-server" || echo ""
    fi
}

stop_server() {
    local pid=$(get_pid)

    if [ -z "$pid" ]; then
        echo "Server is not running"
        return 0
    fi

    echo "Stopping Matrix Server (PID: $pid)..."

    kill -TERM "$pid" 2>/dev/null || true

    local timeout=30
    local count=0

    while kill -0 "$pid" 2>/dev/null && [ $count -lt $timeout ]; do
        sleep 1
        count=$((count + 1))
    done

    if kill -0 "$pid" 2>/dev/null; then
        echo "Server did not stop gracefully, forcing kill..."
        kill -KILL "$pid" 2>/dev/null || true
        sleep 2
    fi

    if [ -f "$PID_FILE" ]; then
        rm -f "$PID_FILE"
    fi

    echo "Server stopped"
}

cleanup_pid_file() {
    if [ -f "$PID_FILE" ]; then
        local pid=$(cat "$PID_FILE")
        if ! kill -0 "$pid" 2>/dev/null; then
            rm -f "$PID_FILE"
            echo "Cleaned up stale PID file"
        fi
    fi
}

status_server() {
    local pid=$(get_pid)

    if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
        echo "Server is running (PID: $pid)"
        return 0
    else
        echo "Server is not running"
        cleanup_pid_file
        return 1
    fi
}

restart_server() {
    stop_server
    sleep 2
    "$SCRIPT_DIR/start_server.sh" start
}

main() {
    case "${1:-stop}" in
        "stop")
            stop_server
            ;;
        "restart")
            restart_server
            ;;
        "status")
            status_server
            ;;
        "force-stop")
            local pid=$(get_pid)
            if [ -n "$pid" ]; then
                kill -KILL "$pid" 2>/dev/null || true
                rm -f "$PID_FILE"
                echo "Server force stopped"
            else
                echo "Server is not running"
            fi
            ;;
        *)
            echo "Usage: $0 {stop|restart|status|force-stop}"
            exit 1
            ;;
    esac
}

main "$@"