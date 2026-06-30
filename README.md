# Docker: ESP-IDF / ESP-IDF Rust container for ESP32

## ディレクトリ構成

```
workspace/
├── docker/          # Docker設定
│   └── compose.yaml
├── scripts/         # 実行スクリプト
│   ├── run_container.sh
│   └── stop_container.sh
├── projects/        # ESP32プロジェクト
│   ├── blink/
│   ├── hello_world/
│   ├── color_led/   # RGB LED (C版)
│   ├── rust/        # Rustプロジェクト (std / esp-idf-hal)
│   │   └── color_led/   # RGB LED (Rust std版)
│   └── hal/         # Rustプロジェクト (no_std / esp-hal)
│       └── color_led/   # RGB LED (Rust no_std版)
└── README.md
```

## Dockerコンテナの起動・停止

### ESP-IDF (C/C++) 環境

```bash
cd workspace
./scripts/run_container.sh        # IDF環境（デフォルト）
./scripts/run_container.sh idf    # 同上
```

### ESP-IDF Rust 環境

```bash
cd workspace
./scripts/run_container.sh rust   # Rust環境
```

### コンテナの停止

```bash
./scripts/stop_container.sh       # 全て停止（デフォルト）
./scripts/stop_container.sh idf   # IDFコンテナのみ停止
./scripts/stop_container.sh rust  # Rustコンテナのみ停止
```

### 使用コンテナについて

| 環境 | イメージ | コンテナ名 |
|------|----------|------------|
| IDF (C/C++) | espressif/idf | esp32_container |
| Rust | espressif/idf-rust:esp32_latest | esp32_rust_container |

Rustには2つの開発方式があり、どちらも同じ `esp32_rust_container` でビルドできる。

| 方式 | 説明 | ターゲット | 主要クレート | 配置 |
|------|------|------------|--------------|------|
| std (esp-idf-hal) | ESP-IDFをRustから利用。WiFi/BLEが容易 | `xtensa-esp32-espidf` | `esp-idf-hal` / `esp-idf-svc` | `projects/rust/` |
| no_std (esp-hal) | ベアメタル。async(embassy)に強い | `xtensa-esp32-none-elf` | `esp-hal` | `projects/hal/` |

- [IDF Docker image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html)
- [ESP-IDF Rust (std)](https://github.com/esp-rs/esp-idf-template)
- [esp-hal (no_std)](https://github.com/esp-rs/esp-hal)

`--privileged` オプションでデバイスアクセスを許可している。

## IDF (C/C++) プロジェクト

コンテナ起動後、`/projects` ディレクトリがマウントされている。

### blink

```bash
cd /projects/blink
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

`menuconfig` の `Example Configuration` でLEDに使用するGPIOを設定可能。

### hello_world

```bash
cd /projects/hello_world
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### color_led (C版)

RGB LEDを虹色に変化させるプロジェクト。

```bash
cd /projects/color_led
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

GPIO設定: GPIO5=Red, GPIO22=Green, GPIO23=Blue

### 空のプロジェクトの作成

```bash
cd /projects
idf.py create-project my_new_project
cd my_new_project
idf.py set-target esp32
```

### テンプレート

```c
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

void app_main(void)
{
    while (true) {
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
```

ヘッダファイルなどは `/opt/esp/idf/components` ディレクトリ以下にある。

## Rust プロジェクト

Rustコンテナ起動後、`/projects` ディレクトリがマウントされている。

### color_led (Rust版)

RGB LEDを虹色に変化させるプロジェクト（C版と同等機能）。

```bash
cd /projects/rust/color_led

# ビルド
cargo build --release

# ESP32に書き込み + モニタ
espflash flash --monitor target/xtensa-esp32-espidf/release/color_led

# シリアルポートを指定する場合
espflash flash --monitor -p /dev/ttyUSB0 target/xtensa-esp32-espidf/release/color_led
```

モニタのみ起動:
```bash
espflash monitor
```

モニタ終了: `Ctrl+]`

### 新規Rustプロジェクトの作成

```bash
cd /projects/rust
cargo generate esp-rs/esp-idf-template cargo
```

プロンプトに従ってプロジェクト名、ターゲット（esp32）などを入力。

### Rustプロジェクト構成

```
project_name/
├── .cargo/
│   └── config.toml      # ビルドターゲット設定
├── Cargo.toml           # 依存関係
├── build.rs             # ビルドスクリプト
├── rust-toolchain.toml  # ツールチェイン指定
├── sdkconfig.defaults   # ESP-IDF設定
└── src/
    └── main.rs          # エントリポイント
```

## Rust no_std (esp-hal) プロジェクト

std版と同じ `esp32_rust_container` を使用する（`./scripts/run_container.sh rust`）。

> **重要:** ビルド前に必ず `source /home/esp/export-esp.sh` を実行すること。
> 実行しないと `linker xtensa-esp32-elf-gcc not found` でビルドに失敗する
> （XtensaのGCCと `LIBCLANG_PATH` をPATHに通すため）。std版では不要だった手順。

### color_led (Rust no_std版)

RGB LEDを虹色に変化させるプロジェクト（C版・std版と同等機能）。

```bash
# コンテナに接続後
source /home/esp/export-esp.sh
cd /projects/hal/color_led

# ビルド
cargo build --release

# ESP32に書き込み + モニタ
espflash flash --monitor target/xtensa-esp32-none-elf/release/color_led
```

GPIO設定: GPIO5=Red, GPIO22=Green, GPIO23=Blue

### 新規 no_std プロジェクトの作成

`esp-generate` を使用する（コンテナに未導入の場合は `cargo install esp-generate` で導入）。

```bash
cd /projects/hal
esp-generate --chip esp32 my_new_project
```

オプション例（LEDC等の周辺機能を使う場合は `unstable-hal` が必要）:

```bash
esp-generate --chip esp32 -o unstable-hal -o esp-backtrace -o log my_new_project
```

> 注意: `esp-generate` はプロジェクト内に `.git` を自動生成する。
> workspace全体のgit管理と競合するため `rm -rf <project>/.git` で削除すること。

### no_std プロジェクト構成

```
project_name/
├── .cargo/
│   └── config.toml      # ターゲット (xtensa-esp32-none-elf), build-std=core
├── Cargo.toml           # 依存関係 (esp-hal 等)
├── build.rs             # linkall.x リンカスクリプト指定
├── rust-toolchain.toml  # channel = "esp"
└── src/
    ├── lib.rs           # ライブラリルート (#![no_std])
    └── bin/
        └── main.rs      # エントリポイント (#![no_std] #![no_main])
```

std版との主な違い: `sdkconfig.defaults` と `ldproxy` が不要。代わりにリンカは
`linkall.x` を直接使用し、ESP-IDF SDKに依存しない。

## 実行中のコンテナにログインする方法

### IDF (C/C++)

```bash
docker exec -it esp32_container bash -c "source /opt/esp/entrypoint.sh && set +e && exec bash"
```

### Rust

```bash
docker exec -it esp32_rust_container bash
```

または `run_container.sh` を再実行しても同様に接続できる。

## License

Apache license.

About esp-idf tools refer to [LICENSE](https://github.com/espressif/esp-idf/blob/master/LICENSE).
