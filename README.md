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
│   └── rust/        # Rustプロジェクト
│       └── color_led/   # RGB LED (Rust版)
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

- [IDF Docker image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html)
- [ESP-IDF Rust](https://github.com/esp-rs/esp-idf-template)

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
