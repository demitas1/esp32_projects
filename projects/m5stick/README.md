# M5Stack StickS3

M5Stack StickS3（ESP32-S3-PICO-1-N8R8）用のファームウェア、PC側ツール、配布用ページ。
開発は PlatformIO（Arduino フレームワーク）＋ M5Unified / M5GFX。

```
m5stick/
├── firmware/   # PlatformIO プロジェクト（アプリごとに env を分ける）
├── host/       # PC側ツール（Python）
├── web/        # 配布用ページ（ESP Web Tools ＋ Web Bluetooth）
├── tools/      # バックアップ等のスクリプト
└── backup/     # 工場出荷時ファームの吸い出し（git管理外）
```

## コンテナ

```bash
cd workspace
./scripts/run_container.sh pio
cd /projects/m5stick/firmware
```

## ダウンロードモード

このプロジェクトのファーム（USB-Serial/JTAG を使用）が動いていれば、`pio run -t upload` の自動リセットで書き込める。
次の場合は、手動でダウンロードモードに入れてから行う。

- 工場出荷時の UiFlow2 が動いているとき（自動リセットを受けると電源が落ちる）
- アプリがクラッシュしている、USB が認識されないとき
- `tools/backup_flash.sh` で吸い出すとき（自動リセットを使わない）

1. USB-C で PC に接続する
2. 側面の電源ボタンを、内部の緑 LED が点滅するまで長押しして離す（画面は消えたまま）
3. `lsusb` で `303a:1001 Espressif USB JTAG/serial debug unit` と表示されれば OK

手動でダウンロードモードに入れた場合、ソフトウェアリセットでは抜けない。
書き込み後は **電源ボタンを1回押して** 起動する（ダブルクリックで電源オフ）。

## ビルド・書き込み

```bash
pio run                       # ビルド（default_envs = bringup）
pio run -e bringup -t upload  # 書き込み
pio device monitor            # シリアルモニタ（終了: Ctrl+C）
```

書き込み前に、同じポートを開いているシリアルモニタ等をすべて閉じること。
他のプロセスが読んでいると `Invalid head of packet` で失敗する（途中で失敗しても再実行すれば復旧する）。

USB は内蔵の USB-Serial/JTAG（`ARDUINO_USB_MODE=1`）を使うため、`lsusb` では常に `303a:1001` と表示され、
アプリ動作中かダウンロードモードかは区別できない。
`Serial.begin()` は自動では呼ばれないので、各アプリの `setup()` で明示的に呼ぶこと
（M5Unified 0.3.0 は `serial_baudrate` の既定値が 0）。

## アプリ

| env | 内容 |
|-----|------|
| `bringup` | ハードウェア動作確認。BtnA（前面）でページ切替、BtnB（側面）でページごとの操作 |

### bringup のページ

| ページ | 確認内容 | BtnB |
|--------|----------|------|
| System | ボード判定、チップ、Flash/PSRAM、バッテリー | - |
| Animation | オフスクリーン描画のアニメーションと fps | ボール数 8→16→32→64 |
| IMU | BMI270 の加速度・ジャイロ | シリアルへ CSV 出力 `imu,ms,ax,ay,az,gx,gy,gz` の開始/停止 |
| Speaker | ES8311 ＋ アンプ | 音階を再生 |
| Mic | 入力レベルと波形 | 2秒録音して再生 |

スピーカーとマイクは ES8311 と I2S を共有するため同時には使えず、ページ切替時に切り替える。

## 工場出荷時ファームのバックアップ・復元

```bash
# ダウンロードモードにしてから
/projects/m5stick/tools/backup_flash.sh            # backup/<日時>_<MAC>/flash.bin に保存（2回読んで照合）

# 復元（ダウンロードモードにしてから）
esptool --chip esp32s3 --port /dev/ttyACM0 --before no-reset write-flash 0x0 backup/<dir>/flash.bin
```

## 参考

- [StickS3 ドキュメント](https://docs.m5stack.com/en/core/StickS3)
- [StickS3 Arduino](https://docs.m5stack.com/en/arduino/m5sticks3/program)
- [M5Unified](https://github.com/m5stack/M5Unified)
