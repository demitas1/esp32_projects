/// Convert HSV color to RGB
///
/// # Arguments
/// * `hue` - Hue value (0-359)
/// * `sat` - Saturation value (0-255)
/// * `val` - Value/brightness (0-255)
///
/// # Returns
/// Tuple of (red, green, blue) values, each 0-255
pub fn hsv_to_rgb(hue: u16, sat: u8, val: u8) -> (u8, u8, u8) {
    if sat == 0 {
        return (val, val, val);
    }

    let region = hue / 60;
    let remainder = ((hue % 60) as u16 * 255 / 60) as u8;

    let p = ((val as u16 * (255 - sat as u16)) / 255) as u8;
    let q = ((val as u16 * (255 - (sat as u16 * remainder as u16) / 255)) / 255) as u8;
    let t = ((val as u16 * (255 - (sat as u16 * (255 - remainder as u16)) / 255)) / 255) as u8;

    match region {
        0 => (val, t, p),
        1 => (q, val, p),
        2 => (p, val, t),
        3 => (p, q, val),
        4 => (t, p, val),
        _ => (val, p, q),
    }
}
