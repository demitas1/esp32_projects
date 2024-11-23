# Docker: esp-idf container for ESP32

## Dockerコンテナの起動

`bash run_container.sh` を使用して起動する.

コンテナ内でCLIツール `esp-idf` が使えるようになる.

ターゲットボードへのダウンロードも可能.

ターゲットボードを接続した状態で起動する (/dev/ttyUSB* にアクセスできることを確認すること)


### 使用コンテナについて

- [IDF Docker image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html)
プロジェクトディレクトリで実行	(`--privileged` に注意)


## プロジェクトのビルドと実行

### blink

- 例

```
cd blink
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p /dev/ttyUSB0 flash
```

- `menuconfig` について

`Example settings` でLEDに使用するGPIOを設定可能.

### hello world

- Build

```
cd hello_world
idf.py set-target esp32
idf.py menuconfig
idf.py build
```

- Run

```
idf.py -p /dev/ttyUSB0 flash
idf.py -p /dev/ttyUSB0 monitor
```


## 空のプロジェクトの作成

コンテナ内で以下実行.

```
idf.py create-project <my_project_name>
idf.py set-target esp32
```

### template

```
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
// #include "esp_event_loop.h"  // deprecated
#include "nvs_flash.h"
#include "driver/gpio.h"

void app_main(void)
{
    while (true) {
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
```

ヘッダファイルなどは `/opt/esp/idf/components` ディレクトリ以下にある.


## 実行中のコンテナにログインする方法

```
docker exec -it esp32_container bash
. /opt/esp/entrypoint.sh
```

## License

Apache license.

About esp-idf tools refer to [LICENSE](https://github.com/espressif/esp-idf/blob/master/LICENSE).
