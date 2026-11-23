# PlatformIO build environment for M5Stack StickS3 (ESP32-S3, Arduino framework)
FROM python:3.12-slim-bookworm

ARG UID=1000
ARG GID=1000
# Empty = latest release. Set e.g. --build-arg PLATFORMIO_VERSION=6.1.18 to pin.
ARG PLATFORMIO_VERSION=

ENV TZ=Asia/Tokyo \
    PLATFORMIO_CORE_DIR=/opt/platformio \
    PIP_NO_CACHE_DIR=1 \
    PIP_DISABLE_PIP_VERSION_CHECK=1

# git: library dependencies fetched from repositories
RUN apt-get update \
    && apt-get install -y --no-install-recommends git ca-certificates tzdata \
    && rm -rf /var/lib/apt/lists/*

# esptool: flash backup and merge_bin for web distribution (standalone from PlatformIO packages)
RUN pip install "platformio${PLATFORMIO_VERSION:+==${PLATFORMIO_VERSION}}" esptool

# Non-root user matching the host UID. dialout grants access to /dev/ttyACM*.
# The core dir is created here so the named volume inherits its ownership.
RUN groupadd -g "${GID}" pio \
    && useradd -m -u "${UID}" -g "${GID}" -G dialout -s /bin/bash pio \
    && mkdir -p "${PLATFORMIO_CORE_DIR}" \
    && chown pio:pio "${PLATFORMIO_CORE_DIR}"

USER pio
WORKDIR /projects
CMD ["bash"]
