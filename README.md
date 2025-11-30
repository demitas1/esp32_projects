# Docker: esp-idf container for ESP32

## ディレクトリ構成

```
workspace/
├── docker/          # Docker設定
│   └── compose.yaml
├── scripts/         # 実行スクリプト
│   └── run_container.sh
├── projects/        # ESP32プロジェクト
│   ├── blink/
│   ├── hello_world/
│   └── ...
└── README.md
```

## Dockerコンテナの起動

```bash
cd workspace
./scripts/run_container.sh
```

コンテナ内でCLIツール `idf.py` が使えるようになる。
ターゲットボードへのダウンロードも可能。

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

コンテナ内で以下を実行。

```bash
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
docker exec -it esp32_container bash
. /opt/esp/entrypoint.sh
```

## License

Apache license.

About esp-idf tools refer to [LICENSE](https://github.com/espressif/esp-idf/blob/master/LICENSE).
