mod hsv;
mod led_pwm;

use esp_idf_hal::prelude::Peripherals;
use std::thread;
use std::time::Duration;

use hsv::hsv_to_rgb;
use led_pwm::RgbLed;

fn main() {
    // Initialize ESP-IDF
    esp_idf_sys::link_patches();
    esp_idf_hal::sys::link_patches();

    let peripherals = Peripherals::take().unwrap();

    let mut rgb_led = RgbLed::new(
        peripherals.ledc.timer0,
        peripherals.ledc.channel0,
        peripherals.ledc.channel1,
        peripherals.ledc.channel2,
        peripherals.pins.gpio5,
        peripherals.pins.gpio22,
        peripherals.pins.gpio23,
    )
    .expect("Failed to initialize RGB LED");

    println!("Rainbow LED Start");

    let mut hue: u16 = 0;

    loop {
        let (r, g, b) = hsv_to_rgb(hue, 255, 255);
        rgb_led.set_color(r, g, b).unwrap();

        hue = (hue + 1) % 360;
        thread::sleep(Duration::from_millis(50));
    }
}
