#!/bin/bash
cd "$(dirname "$0")/../docker"
docker compose up -d
docker exec -it esp32_container bash
