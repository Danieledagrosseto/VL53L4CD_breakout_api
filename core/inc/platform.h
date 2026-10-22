/*
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef VL53L4CD_PLATFORM_H_
#define VL53L4CD_PLATFORM_H_

#include <stdint.h>

/*
 * Define VL53L4CD_I2C_FAST_MODE_PLUS to enable the sensor's 1 MHz I2C mode.
 * The MCU I2C peripheral must also be initialized for Fast Mode Plus.
 */

typedef uint16_t Dev_t;

typedef uint8_t VL53L4CD_Error;

#define TRANSFER_CMPLT (0x00u)
#define TRANSFER_ERROR (0xFFu)

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t register_address,
                         uint32_t *value);
uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t register_address,
                        uint16_t *value);
uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t register_address,
                        uint8_t *value);
uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t register_address,
                        uint8_t value);
uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t register_address,
                        uint16_t value);
uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t register_address,
                         uint32_t value);
uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t time_ms);

#endif
