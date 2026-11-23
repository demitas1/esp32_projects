#!/bin/bash
# Back up the entire flash of an M5Stack StickS3 (read-only, nothing is written).
# Run inside esp32_pio_container.
#
# The device must be in download mode BEFORE running this script:
#   connect USB, hold the side power button until the green LED blinks, then release
#   (lsusb shows 303a:1001 "USB JTAG/serial debug unit").
# Auto-reset is not used: resetting from the UiFlow2 firmware powers the StickS3 off.
#
# Usage: ./backup_flash.sh [port] [size]
#   port: serial port (default: /dev/ttyACM0)
#   size: flash size in bytes or ALL (default: ALL = detected size)
#
# Output: ../backup/<timestamp>_<mac>/
#   flash.bin           full flash image (restore: esptool write-flash 0x0 flash.bin)
#   partitions.csv      decoded partition table
#   flash.bin.sha256    checksum (verified by a second read)
#   info.txt            chip information

set -euo pipefail

PORT="${1:-/dev/ttyACM0}"
SIZE="${2:-ALL}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BACKUP_ROOT="$(dirname "$SCRIPT_DIR")/backup"

esp() {
    esptool --chip esp32s3 --port "$PORT" "$@"
}

echo "== Reading chip info from ${PORT}"
INFO="$(esp --before no-reset --after no-reset flash-id 2>&1)" || { echo "$INFO"; exit 1; }
MAC="$(echo "$INFO" | awk '/^MAC:/ {print $2; exit}' | tr -d ':')"
[ -n "$MAC" ] || { echo "$INFO"; echo "Failed to read MAC address (is the device in download mode?)" >&2; exit 1; }

OUT="${BACKUP_ROOT}/$(date +%Y%m%d_%H%M%S)_${MAC}"
mkdir -p "$OUT"
echo "$INFO" > "${OUT}/info.txt"

echo "== Reading flash (${SIZE}) -> ${OUT}/flash.bin"
esp --before no-reset --after no-reset read-flash 0 "$SIZE" "${OUT}/flash.bin"

echo "== Verifying by a second read"
TMP="$(mktemp)"
trap 'rm -f "$TMP"' EXIT
esp --before no-reset --after no-reset read-flash 0 "$SIZE" "$TMP"
if ! cmp -s "${OUT}/flash.bin" "$TMP"; then
    echo "Verification failed: two reads differ" >&2
    exit 1
fi
(cd "$OUT" && sha256sum flash.bin > flash.bin.sha256)

echo "== Decoding partition table"
python3 - "${OUT}/flash.bin" > "${OUT}/partitions.csv" <<'EOF'
import struct, sys

TYPES = {0: "app", 1: "data"}
with open(sys.argv[1], "rb") as f:
    f.seek(0x8000)
    table = f.read(0xC00)

print("# Name, Type, SubType, Offset, Size, Flags")
for i in range(0, len(table), 32):
    magic, ptype, subtype, offset, size, label, flags = struct.unpack("<HBBII16sI", table[i:i + 32])
    if magic != 0x50AA:
        break
    name = label.rstrip(b"\0").decode(errors="replace")
    print(f"{name}, {TYPES.get(ptype, hex(ptype))}, {subtype:#04x}, {offset:#x}, {size:#x}, {flags:#x}")
EOF

# A software reset keeps the StickS3 in download mode; a power-button press is needed.
echo "== Done. Single-press the power button to boot the firmware again."

echo
cat "${OUT}/partitions.csv"
echo
echo "Backup complete: ${OUT}"
ls -l "$OUT"
