/*
 * Generic MCU transport for the EZrange VL53L4CD forwarding breakout.
 * The breakout packet framing is specific to this bridge firmware.
 */

#include "VL53L4CD_api.h"
#include "vl53l4cd_i2c_platform.h"

#include <stddef.h>

#define VL53L4CD_BREAKOUT_WAIT_US 150u
#define VL53L4CD_ADDRESS_WRITE_WAIT_MS 25u
#define VL53L4CD_MAX_REGISTER_DATA 4u
#define VL53L4CD_READ_COMMAND_SIZE 3u

static int32_t vl53l4cd_validate_device(Dev_t dev, uint8_t *address_7bit)
{
    if ((address_7bit == NULL) || (dev < 0x10u) || (dev > 0xEEu) ||
        ((dev & 1u) != 0u))
    {
        return 1;
    }

    *address_7bit = (uint8_t)(dev >> 1);
    return 0;
}

static uint8_t vl53l4cd_wait_us(uint32_t time_us)
{
    return (VL53L4CD_Port_DelayUs(time_us) == 0) ?
           TRANSFER_CMPLT : TRANSFER_ERROR;
}

static uint8_t vl53l4cd_write_register(Dev_t dev,
                                       uint16_t register_address,
                                       const uint8_t *data,
                                       uint32_t count)
{
    uint8_t packet[3u + VL53L4CD_MAX_REGISTER_DATA];
    uint8_t address_7bit;

    if ((count > VL53L4CD_MAX_REGISTER_DATA) ||
        ((count > 0u) && (data == NULL)) ||
        (vl53l4cd_validate_device(dev, &address_7bit) != 0))
    {
        return TRANSFER_ERROR;
    }

    packet[0] = 0u;
    packet[1] = (uint8_t)(register_address >> 8);
    packet[2] = (uint8_t)register_address;
    for (uint32_t i = 0u; i < count; i++)
    {
        packet[3u + i] = data[i];
    }

    if ((VL53L4CD_Port_I2C_Write(address_7bit, packet, 3u + count) != 0) ||
        (vl53l4cd_wait_us(VL53L4CD_BREAKOUT_WAIT_US) != TRANSFER_CMPLT))
    {
        return TRANSFER_ERROR;
    }

    if ((register_address == VL53L4CD_I2C_SLAVE__DEVICE_ADDRESS) &&
        (VL53L4CD_Port_DelayMs(VL53L4CD_ADDRESS_WRITE_WAIT_MS) != 0))
    {
        return TRANSFER_ERROR;
    }

    return TRANSFER_CMPLT;
}

static uint8_t vl53l4cd_read_register(Dev_t dev,
                                      uint16_t register_address,
                                      uint8_t *data,
                                      uint32_t count)
{
    uint8_t command[VL53L4CD_READ_COMMAND_SIZE];
    uint8_t address_7bit;

    if ((count == 0u) || (count > VL53L4CD_MAX_REGISTER_DATA) ||
        (data == NULL) ||
        (vl53l4cd_validate_device(dev, &address_7bit) != 0))
    {
        return TRANSFER_ERROR;
    }

    command[0] = (uint8_t)count;
    command[1] = (uint8_t)(register_address >> 8);
    command[2] = (uint8_t)register_address;

    if ((VL53L4CD_Port_I2C_Write(address_7bit, command,
                                 VL53L4CD_READ_COMMAND_SIZE) != 0) ||
        (vl53l4cd_wait_us(VL53L4CD_BREAKOUT_WAIT_US) != TRANSFER_CMPLT) ||
        (VL53L4CD_Port_I2C_Read(address_7bit, data, count) != 0) ||
        (vl53l4cd_wait_us(VL53L4CD_BREAKOUT_WAIT_US) != TRANSFER_CMPLT))
    {
        return TRANSFER_ERROR;
    }

    return TRANSFER_CMPLT;
}

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t register_address,
                         uint32_t *value)
{
    uint8_t data[4];

    if (value == NULL)
    {
        return TRANSFER_ERROR;
    }

    if (vl53l4cd_read_register(dev, register_address, data, sizeof(data)) !=
        TRANSFER_CMPLT)
    {
        return TRANSFER_ERROR;
    }

    *value = ((uint32_t)data[0] << 24) |
             ((uint32_t)data[1] << 16) |
             ((uint32_t)data[2] << 8) |
             (uint32_t)data[3];
    return TRANSFER_CMPLT;
}

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t register_address,
                        uint16_t *value)
{
    uint8_t data[2];

    if (value == NULL)
    {
        return TRANSFER_ERROR;
    }

    if (vl53l4cd_read_register(dev, register_address, data, sizeof(data)) !=
        TRANSFER_CMPLT)
    {
        return TRANSFER_ERROR;
    }

    *value = (uint16_t)(((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    return TRANSFER_CMPLT;
}

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t register_address,
                        uint8_t *value)
{
    if (value == NULL)
    {
        return TRANSFER_ERROR;
    }

    return vl53l4cd_read_register(dev, register_address, value, 1u);
}

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t register_address, uint8_t value)
{
    return vl53l4cd_write_register(dev, register_address, &value, 1u);
}

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t register_address,
                        uint16_t value)
{
    uint8_t data[2];

    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)value;
    return vl53l4cd_write_register(dev, register_address, data, sizeof(data));
}

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t register_address,
                         uint32_t value)
{
    uint8_t data[4];

    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)(value >> 16);
    data[2] = (uint8_t)(value >> 8);
    data[3] = (uint8_t)value;
    return vl53l4cd_write_register(dev, register_address, data, sizeof(data));
}

uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t time_ms)
{
    (void)dev;
    return (VL53L4CD_Port_DelayMs(time_ms) == 0) ?
           TRANSFER_CMPLT : TRANSFER_ERROR;
}
