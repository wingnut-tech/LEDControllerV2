# FT Night Radian LED Controller

## Configuration:

The firmware comes set up for the FT Night Radian by default. If you have a different layout of LED strips, you can use the [LED Controller Configurator](https://wingnut-tech.github.io/LEDControllerConfigurator/) to create a new layout.

After configuring, you can download the whole firmware plus config, or just the config to apply to an already-flashed board (assuming the flashed firmware is a compatible version).

Note: v2 boards allow configuring both the layout and the enabled/disabled shows directly from the configurator page. v1 boards only allow layout configuration. You can still enable/disable shows on both boards using the buttons.

---

## Flashing:

### v2 boards:
- Download the .uf2 file (from the latest [release](https://github.com/wingnut-tech/LEDControllerV2/releases), or from the configurator page)
- Hold down the "Boot" button on the board while plugging it in to your computer via USB
- Drag-and-drop or copy/paste the .uf2 file into the "RPI-RP2" drive that shows up


### v1 boards:
- Download the .hex file (from the latest [release](https://github.com/wingnut-tech/LEDControllerV2/releases), or from the configurator page)
- Download and run the [HEX Uploader](https://github.com/wingnut-tech/HEXUploader) to upload the .hex file to your board

---

## Building/Development

### Setup
- `git submodule init`
- `git submodule update`

### Build

- `./build.sh new`

## Libraries used

- BMX280 library from <https://github.com/Erriez/ErriezBMX280> (v1.0.1)
- FastLED library from <https://github.com/FastLED/FastLED> (v3.6.0)
