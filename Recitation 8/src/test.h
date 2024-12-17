#include <stdint.h>

#define AD5933_I2C_ADDRESS 0x0D      // 7-bit I2C address of AD5933
#define CONTROL_REG 0x80             // Control register address
#define STATUS_REG 0x8F              // Status register address
#define TEMPERATURE_REG_MSB 0x92     // Register address for temperature MSB
#define TEMPERATURE_REG_LSB 0x93     // Register address for temperature LSB
#define START_TEMP_MEAS_CMD 0x90     // Command to start temperature measurement
#define TEMP_MEAS_COMPLETE_FLAG 0x01 // Temperature measurement complete flag
#define SUCCESS 1
#define FAILURE 0

float GetTemperature()
{
    uint8_t msb, lsb;  // MSB and LSB of temperature data
    uint8_t status;    // Status register value
    uint8_t buffer[2]; // Raw temperature register values

    // Step 1: Initialize the I2C hardware
    if (Start_I2C() != SUCCESS)
    {
        return -999.0;
    }

    // Step 2: Issue command to start temperature measurement
    // Send start condition followed by the address of the slave with write
    if (I2C_Send_Start_Condition(AD5933_I2C_ADDRESS, 0) != SUCCESS)
    {
        return -999.0;
    }

    // Send write register and write data
    // Write to the control register with the command to start temperature measurement
    if (I2C_Write_Byte(CONTROL_REG) != SUCCESS || I2C_Write_Byte(START_TEMP_MEAS_CMD) != SUCCESS)
    {
        I2C_Send_Stop_Condition();
        return -999.0;
    }

    // Send a stop condition after issuing the command
    if (I2C_Send_Stop_Condition() != SUCCESS)
    {
        return -999.0;
    }

    // Step 3: Poll the Status Register until temperature measurement is complete
    do
    {
        // Send a start condition to read the status register
        if (I2C_Send_Start_Condition(AD5933_I2C_ADDRESS, 0) != SUCCESS)
        {
            return -999.0;
        }

        // Write the status register address
        if (I2C_Write_Byte(STATUS_REG) != SUCCESS)
        {
            I2C_Send_Stop_Condition();
            return -999.0;
        }

        // Restart to read the status register value
        if (I2C_Send_Start_Condition(AD5933_I2C_ADDRESS, 1) != SUCCESS)
        {
            I2C_Send_Stop_Condition();
            return -999.0;
        }

        // Read the status register (1 byte)
        if (I2C_Request_Read(&status, 1) != SUCCESS)
        {
            I2C_Send_Stop_Condition();
            return -999.0;
        }

        I2C_Send_Stop_Condition(); // Send stop condition after each read
    } while (!(status & TEMP_MEAS_COMPLETE_FLAG)); // Check if temperature measurement is complete

    // Instead of polling, a delay of 800us can be used. AD5933 does not support interrupt mechanism.

    // Step 4: Read MSB and LSB of temperature data
    if (I2C_Send_Start_Condition(AD5933_I2C_ADDRESS, 0) != SUCCESS)
    {
        return -999.0;
    }

    // Write the register address for MSB
    if (I2C_Write_Byte(TEMPERATURE_REG_MSB) != SUCCESS)
    {
        I2C_Send_Stop_Condition();
        return -999.0;
    }

    // Restart the communication to read data
    if (I2C_Send_Start_Condition(AD5933_I2C_ADDRESS, 1) != SUCCESS)
    {
        I2C_Send_Stop_Condition();
        return -999.0;
    }

    // Read 2 bytes (MSB and LSB) of temperature data
    if (I2C_Request_Read(buffer, 2) != SUCCESS)
    {
        I2C_Send_Stop_Condition();
        return -999.0;
    }

    // Extract MSB and LSB from buffer
    msb = buffer[0] & 0x3F; // Mask the two MSB dontcare bits (Bits [7:6])
    lsb = buffer[1];

    // Send a stop condition to release the I2C bus
    if (I2C_Send_Stop_Condition() != SUCCESS)
    {
        return -999.0;
    }

    // Step 5: Combine MSB and LSB into a 14-bit raw temperature value
    uint16_t rawTemperature = (msb << 8) | lsb;

    // Convert the raw temperature to Celsius: Temp = (Raw / 32) °C
    float temperature;
    if (rawTemperature & 0x2000) // Check if temperature is negative (Bit 13 is set)
    {
        temperature = ((float)(rawTemperature - 16384) / 32.0);
    }
    else // Positive temperature
    {
        temperature = (float)rawTemperature / 32.0;
    }

    return temperature; // Return the calculated temperature
}
