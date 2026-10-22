#ifndef VL53L4CD_I2C_PLATFORM_H_
#define VL53L4CD_I2C_PLATFORM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Implement these hooks with the target MCU's I2C and delay facilities.
 * Return zero on success and nonzero on failure. I2C transfers must complete
 * synchronously, use the 7-bit address, transfer exactly count bytes, end in
 * STOP, and have a bounded timeout.
 *
 * A speed of zero selects the MCU driver's configured default speed.
 */
int32_t VL53L4CD_Port_I2C_Init(uint16_t speed_khz);
int32_t VL53L4CD_Port_I2C_Close(void);
int32_t VL53L4CD_Port_I2C_Write(uint8_t address_7bit,
                                const uint8_t *data, uint32_t count);
int32_t VL53L4CD_Port_I2C_Read(uint8_t address_7bit,
                               uint8_t *data, uint32_t count);
int32_t VL53L4CD_Port_DelayMs(uint32_t delay_ms);
int32_t VL53L4CD_Port_DelayUs(uint32_t delay_us);

#ifdef __cplusplus
}
#endif

#endif
