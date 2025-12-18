# Docker: esp-idf container for ESP32

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
│   └── ...
└── README.md
```

## Dockerコンテナの起動・停止

```bash
cd workspace
./scripts/run_container.sh   # 起動＋接続（IDF環境自動有効化）
./scripts/stop_container.sh  # 停止
```

`run_container.sh` はコンテナ起動後、自動的にESP-IDF環境を有効化してシェルに接続する。
コンテナ内でCLIツール `idf.py` がすぐに使える。

### 使用コンテナについて

- [IDF Docker image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html)

`--privileged` オプションでデバイスアクセスを許可している。

## プロジェクトのビルドと実行

コンテナ起動後、`/projects` ディレクトリがマウントされている。

### blink

```bash
cd blink
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

`menuconfig` の `Example Configuration` でLEDに使用するGPIOを設定可能。

### hello_world

```bash
cd hello_world
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p /dev/ttyUSB0 flash
idf.py -p /dev/ttyUSB0 monitor
```

## 空のプロジェクトの作成

`run_container.sh` でコンテナに接続後、以下を実行。

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

## 実行中のコンテナにログインする方法

```bash
docker exec -it esp32_container bash -c "source /opt/esp/entrypoint.sh && set +e && exec bash"
```

または `run_container.sh` を再実行しても同様に接続できる。

## License

Apache license.

About esp-idf tools refer to [LICENSE](https://github.com/espressif/esp-idf/blob/master/LICENSE).
