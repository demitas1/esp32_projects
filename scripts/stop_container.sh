#!/bin/bash

# Usage: ./stop_container.sh [idf|rust|all]
# Default: all

cd "$(dirname "$0")/../docker"

TARGET="${1:-all}"

case "$TARGET" in
    idf)
        docker compose stop esp-idf
        ;;
    rust)
        docker compose stop esp-idf-rust
        ;;
    all)
        docker compose down
        ;;
    *)
        echo "Usage: $0 [idf|rust|all]"
        echo "  idf  - Stop ESP-IDF container"
        echo "  rust - Stop ESP-IDF Rust container"
        echo "  all  - Stop all containers (default)"
        exit 1
        ;;
esac
