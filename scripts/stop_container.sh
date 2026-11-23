#!/bin/bash

# Usage: ./stop_container.sh [idf|rust|pio|all]
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
    pio)
        docker compose stop esp32-pio
        ;;
    all)
        docker compose down
        ;;
    *)
        echo "Usage: $0 [idf|rust|pio|all]"
        echo "  idf  - Stop ESP-IDF container"
        echo "  rust - Stop ESP-IDF Rust container"
        echo "  pio  - Stop PlatformIO container"
        echo "  all  - Stop all containers (default)"
        exit 1
        ;;
esac
