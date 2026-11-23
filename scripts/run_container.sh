#!/bin/bash

# Usage: ./run_container.sh [idf|rust|pio]
# Default: idf

cd "$(dirname "$0")/../docker"

TARGET="${1:-idf}"

case "$TARGET" in
    idf)
        SERVICE="esp-idf"
        CONTAINER="esp32_container"
        INIT_CMD="source /opt/esp/entrypoint.sh && set +e && exec bash"
        ;;
    rust)
        SERVICE="esp-idf-rust"
        CONTAINER="esp32_rust_container"
        INIT_CMD="exec bash"
        ;;
    pio)
        SERVICE="esp32-pio"
        CONTAINER="esp32_pio_container"
        INIT_CMD="exec bash"
        ;;
    *)
        echo "Usage: $0 [idf|rust|pio]"
        echo "  idf  - ESP-IDF (C/C++) environment (default)"
        echo "  rust - ESP-IDF Rust environment"
        echo "  pio  - PlatformIO environment (M5Stack StickS3)"
        exit 1
        ;;
esac

docker compose up -d "$SERVICE"
docker exec -it "$CONTAINER" bash -c "$INIT_CMD"
