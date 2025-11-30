#!/bin/bash
cd "$(dirname "$0")/../docker"
docker compose run --rm esp-idf
