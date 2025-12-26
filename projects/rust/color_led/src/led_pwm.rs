use esp_idf_hal::ledc::{config::TimerConfig, LedcDriver, LedcTimerDriver, Resolution};
use esp_idf_hal::ledc::{CHANNEL0, CHANNEL1, CHANNEL2, TIMER0};
use esp_idf_hal::gpio::{Gpio5, Gpio22, Gpio23};
use esp_idf_hal::peripheral::Peripheral;
use esp_idf_hal::prelude::*;
use esp_idf_hal::sys::EspError;

pub struct RgbLed<'a> {
    red: LedcDriver<'a>,
    green: LedcDriver<'a>,
    blue: LedcDriver<'a>,
}

impl<'a> RgbLed<'a> {
    pub fn new(
        timer: impl Peripheral<P = TIMER0> + 'a,
        channel_r: impl Peripheral<P = CHANNEL0> + 'a,
        channel_g: impl Peripheral<P = CHANNEL1> + 'a,
        channel_b: impl Peripheral<P = CHANNEL2> + 'a,
        pin_r: impl Peripheral<P = Gpio5> + 'a,
        pin_g: impl Peripheral<P = Gpio22> + 'a,
        pin_b: impl Peripheral<P = Gpio23> + 'a,
    ) -> Result<Self, EspError> {
        let timer_config = TimerConfig::default()
            .frequency(100.Hz())
            .resolution(Resolution::Bits8);

        let timer_driver = LedcTimerDriver::new(timer, &timer_config)?;

        let red = LedcDriver::new(channel_r, &timer_driver, pin_r)?;
        let green = LedcDriver::new(channel_g, &timer_driver, pin_g)?;
        let blue = LedcDriver::new(channel_b, &timer_driver, pin_b)?;

        Ok(Self { red, green, blue })
    }

    pub fn set_color(&mut self, r: u8, g: u8, b: u8) -> Result<(), EspError> {
        self.red.set_duty(r as u32)?;
        self.green.set_duty(g as u32)?;
        self.blue.set_duty(b as u32)?;
        Ok(())
    }
}
