# color_led (Rust no_std / esp-hal)

ESP32のRGB LEDをHSV色相に沿って虹色に変化させるプロジェクト。
`esp-hal`（no_std・ベアメタル）で実装している。

C版（`projects/color_led`）、Rust std版（`projects/rust/color_led`）と同等の機能で、
HSV→RGB変換アルゴリズムは共通。

## 動作

- GPIO5=Red, GPIO22=Green, GPIO23=Blue を LEDC (PWM) で駆動
- 色相(hue)を50ms毎に1度進め、虹色に連続変化させる
- PWM: 低速チャンネル・8bit分解能・1kHz

## ビルド・書き込み

`espressif/idf-rust` コンテナ（`esp32_rust_container`）内で実行する。

> **重要:** ビルド前に必ず `source /home/esp/export-esp.sh` を実行すること。
> 実行しないと `linker xtensa-esp32-elf-gcc not found` で失敗する。

```bash
source /home/esp/export-esp.sh
cd /projects/hal/color_led

# ビルド
cargo build --release

# ESP32に書き込み + モニタ
espflash flash --monitor target/xtensa-esp32-none-elf/release/color_led
```

モニタ終了: `Ctrl+]`

## 構成

| ファイル | 内容 |
|---------|------|
| `src/lib.rs` | ライブラリルート（`#![no_std]`）、hsvモジュール公開 |
| `src/hsv.rs` | HSV→RGB変換（整数演算） |
| `src/bin/main.rs` | エントリポイント。LEDC初期化とメインループ |
| `.cargo/config.toml` | ターゲット `xtensa-esp32-none-elf`、`build-std = ["core"]` |
| `build.rs` | `linkall.x` リンカスクリプト指定 |
| `rust-toolchain.toml` | `channel = "esp"` |

## 主要クレート

- `esp-hal` — no_std ハードウェア抽象化レイヤー（`unstable` 機能でLEDCを使用）
- `esp-backtrace` — パニックハンドラ
- `esp-println` / `log` — ログ出力
- `esp-bootloader-esp-idf` — ESP-IDFブートローダ用app-descriptor生成
