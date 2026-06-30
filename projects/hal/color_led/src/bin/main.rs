#![no_std]
#![no_main]
#![deny(
    clippy::mem_forget,
    reason = "mem::forget is generally not safe to do with esp_hal types, especially those \
    holding buffers for the duration of a data transfer."
)]
#![deny(clippy::large_stack_frames)]

use color_led::hsv::hsv_to_rgb;
use esp_backtrace as _;
use esp_hal::clock::CpuClock;
use esp_hal::delay::Delay;
use esp_hal::gpio::DriveMode;
use esp_hal::ledc::channel::{self, ChannelIFace};
use esp_hal::ledc::timer::{self, TimerIFace};
use esp_hal::ledc::{LSGlobalClkSource, Ledc, LowSpeed};
use esp_hal::main;
use esp_hal::time::Rate;
use log::info;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

// RGB LED pin assignment (matches the C / esp-idf-hal versions).
// R = GPIO5, G = GPIO22, B = GPIO23

/// Scale an 8-bit color component (0-255) to a duty percentage (0-100).
fn to_duty_pct(component: u8) -> u8 {
    (component as u32 * 100 / 255) as u8
}

#[allow(
    clippy::large_stack_frames,
    reason = "it's not unusual to allocate larger buffers etc. in main"
)]
#[main]
fn main() -> ! {
    // generator version: 1.3.0
    // generator parameters: --chip esp32 -o unstable-hal -o esp-backtrace -o log -o vscode

    esp_println::logger::init_logger_from_env();

    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // Configure the LEDC peripheral for PWM dimming.
    let mut ledc = Ledc::new(peripherals.LEDC);
    ledc.set_global_slow_clock(LSGlobalClkSource::APBClk);

    // Single shared low-speed timer: 8-bit resolution at 1 kHz.
    let mut lstimer0 = ledc.timer::<LowSpeed>(timer::Number::Timer0);
    lstimer0
        .configure(timer::config::Config {
            duty: timer::config::Duty::Duty8Bit,
            clock_source: timer::LSClockSource::APBClk,
            frequency: Rate::from_khz(1),
        })
        .expect("Failed to configure LEDC timer");

    // One channel per color, all driven by the shared timer.
    let mut channel_r = ledc.channel(channel::Number::Channel0, peripherals.GPIO5);
    let mut channel_g = ledc.channel(channel::Number::Channel1, peripherals.GPIO22);
    let mut channel_b = ledc.channel(channel::Number::Channel2, peripherals.GPIO23);

    for channel in [&mut channel_r, &mut channel_g, &mut channel_b] {
        channel
            .configure(channel::config::Config {
                timer: &lstimer0,
                duty_pct: 0,
                drive_mode: DriveMode::PushPull,
            })
            .expect("Failed to configure LEDC channel");
    }

    info!("Rainbow LED Start");

    let delay = Delay::new();
    let mut hue: u16 = 0;

    loop {
        let (r, g, b) = hsv_to_rgb(hue, 255, 255);
        channel_r.set_duty(to_duty_pct(r)).ok();
        channel_g.set_duty(to_duty_pct(g)).ok();
        channel_b.set_duty(to_duty_pct(b)).ok();

        hue = (hue + 1) % 360;
        delay.delay_millis(50);
    }
}
