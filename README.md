# EZrange VL53L4CD breakout API for MCU ports

This directory contains the ST VL53L4CD Ultra Lite Driver (ULD) and a
portable MCU transport for the EZrange breakout. The breakout acts as an I2C
bridge: its host-facing address is forwarded to the VL53L4CD sensor, and
measurement data are produced by the sensor.

| Front | Back |
| --- | --- |
| ![Front of the EZrange VL53L4CD breakout board](pictures/fronte.jpg) | ![Back of the EZrange VL53L4CD breakout board](pictures/retro.jpg) |

[Breakout schematic](pictures/breakout%20schematics.pdf)

## Build selection

Compile the two driver files and the MCU transport:

- `core/src/VL53L4CD_api.c`
- `core/src/VL53L4CD_calibration.c`
- `platform/src/vl53l4cd_i2c_platform.c`

Add `core/inc` and `platform/inc` to the compiler include paths. Do not add
the PSoC project’s `platform.c`: the file in this repository replaces its
PSoC-specific I2C, timing, and GPIO dependencies. The driver source is the
VL53L4CD ULD 2.2.2 from the supplied PSoC project; its ST copyright and license
notices are retained.

## MCU adapter

Implement the six `VL53L4CD_Port_*` hooks declared in
`platform/inc/vl53l4cd_i2c_platform.h` with the target MCU's I2C and delay
facilities. Each hook returns zero on success and nonzero on failure. I2C
transfers must block until complete, use the supplied 7-bit address, transfer
exactly the requested number of bytes, and finish with STOP. Use a bounded
timeout so a stalled bus cannot block forever. `VL53L4CD_Port_I2C_Init` takes
the requested speed in kHz; zero means use the MCU driver's default.

Initialize the MCU I2C peripheral using `VL53L4CD_Port_I2C_Init`, then pass the
breakout's 8-bit shifted I2C address as `Dev_t` (for example, `0x52` when the
breakout's 7-bit host address is `0x29`). `VL53L4CD_SetI2CAddress` changes the
address stored by the bridge; update the `Dev_t` passed to subsequent calls
after changing it.

## Breakout transport

The bridge uses 16-bit register addresses and a bridge-specific packet format.
Writes send `0`, register high byte, register low byte, then the data bytes.
Reads send the byte count and 16-bit register address, wait 150 microseconds,
read the response, then wait another 150 microseconds. Writes to the sensor
address register also wait 25 ms for the bridge's EEPROM update. These delays
match the PSoC implementation and should be validated against the board
firmware before changing them.

The MCU delay hooks must support both milliseconds and microseconds; unlike
the VL53L0X port, rounding microsecond waits up to a millisecond would not
preserve this bridge's read timing.
