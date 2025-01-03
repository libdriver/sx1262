/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_sx1262.c
 * @brief     driver sx1262 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2024-10-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2024/10/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_sx1262.h"
#include <math.h>

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Semtech SX1262"        /**< chip name */
#define MANUFACTURER_NAME         "Semtech"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.8f                    /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.7f                    /**< chip max supply voltage */
#define MAX_CURRENT               107.0f                  /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                  /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                   /**< chip max operating temperature */
#define DRIVER_VERSION            1000                    /**< driver version */

/**
 * @brief chip command definition
 */
#define SX1262_COMMAND_SET_SLEEP                         0x84        /**< set sleep command */
#define SX1262_COMMAND_SET_STANDBY                       0x80        /**< set standby command */
#define SX1262_COMMAND_SET_FS                            0xC1        /**< set fs command */
#define SX1262_COMMAND_SET_TX                            0x83        /**< set tx command */
#define SX1262_COMMAND_SET_RX                            0x82        /**< set rx command */
#define SX1262_COMMAND_STOP_TIMER_ON_PREAMBLE            0x9F        /**< stop timer on preamble command */
#define SX1262_COMMAND_SET_RX_DUTY_CYCLE                 0x94        /**< set rx duty cycle command */
#define SX1262_COMMAND_SET_CAD                           0xC5        /**< set cad command */
#define SX1262_COMMAND_SET_TX_CONTINUOUS_WAVE            0xD1        /**< set tx continuous wave command */
#define SX1262_COMMAND_SET_TX_INFINITE_PREAMBLE          0xD2        /**< set tx infinite preamble command */
#define SX1262_COMMAND_SET_REGULATOR_MODE                0x96        /**< set regulator mode command */
#define SX1262_COMMAND_SET_CALIBRATE                     0x89        /**< set calibrate command */
#define SX1262_COMMAND_SET_CALIBRATE_IMAGE               0x98        /**< set calibrate image command */
#define SX1262_COMMAND_SET_PA_CONFIG                     0x95        /**< set pa config command */
#define SX1262_COMMAND_SET_RX_TX_FALLBACK_MODE           0x93        /**< set rx tx fallback mode command */
#define SX1262_COMMAND_WRITE_REGISTER                    0x0D        /**< write register command */
#define SX1262_COMMAND_READ_REGISTER                     0x1D        /**< read register command */
#define SX1262_COMMAND_WRITE_BUFFER                      0x0E        /**< write buffer command */
#define SX1262_COMMAND_READ_BUFFER                       0x1E        /**< read buffer command */
#define SX1262_COMMAND_SET_DIO_IRQ_PARAMS                0x08        /**< set dio irq params command */
#define SX1262_COMMAND_GET_IRQ_STATUS                    0x12        /**< get irq status command */
#define SX1262_COMMAND_CLEAR_IRQ_STATUS                  0x02        /**< clear irq status command */
#define SX1262_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL        0x9D        /**< set dio2 as rf switch ctrl command */
#define SX1262_COMMAND_SET_DIO3_AS_TCXO_CTRL             0x97        /**< set dio3 as tcxo ctrl command */
#define SX1262_COMMAND_SET_RF_FREQUENCY                  0x86        /**< set rf frequency command */
#define SX1262_COMMAND_SET_PACKET_TYPE                   0x8A        /**< set packet type command */
#define SX1262_COMMAND_GET_PACKET_TYPE                   0x11        /**< get packet type command */
#define SX1262_COMMAND_SET_TX_PARAMS                     0x8E        /**< set tx params command */
#define SX1262_COMMAND_SET_MODULATION_PARAMS             0x8B        /**< set modulation params command */
#define SX1262_COMMAND_SET_PACKET_PARAMS                 0x8C        /**< set packet params command */
#define SX1262_COMMAND_SET_CAD_PARAMS                    0x88        /**< set cad params command */
#define SX1262_COMMAND_SET_BUFFER_BASE_ADDRESS           0x8F        /**< set buffer base address command */
#define SX1262_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT         0xA0        /**< set lora symb num timeout command */
#define SX1262_COMMAND_GET_STATUS                        0xC0        /**< get status command */
#define SX1262_COMMAND_GET_RSSI_LNST                     0x15        /**< get rssi inst command */
#define SX1262_COMMAND_GET_RX_BUFFER_STATUS              0x13        /**< get rx buffer status command */
#define SX1262_COMMAND_GET_PACKET_STATUS                 0x14        /**< get packet status command */
#define SX1262_COMMAND_GET_DEVICE_ERRORS                 0x17        /**< get device errors command */
#define SX1262_COMMAND_CLEAR_DEVICE_ERRORS               0x07        /**< clear device errors command */
#define SX1262_COMMAND_GET_STATS                         0x10        /**< get stats command */
#define SX1262_COMMAND_RESET_STATS                       0x00        /**< reset stats command */

/**
 * @brief chip register definition
 */
#define SX1262_REG_HOPPING_ENABLE                        0x0385      /**< hopping enable register */
#define SX1262_REG_PACKET_LENGTH                         0x0386      /**< packet length register */
#define SX1262_REG_NB_HOPPING_BLOCKS                     0x0387      /**< nb hopping blocks register */
#define SX1262_REG_NB_SYMBOLS_0                          0x0388      /**< nb symbols 0 register */
#define SX1262_REG_FREQ_0                                0x038A      /**< freq 0 register */
#define SX1262_REG_NB_SYMBOLS_15                         0x03E2      /**< nb symbols 15 register */
#define SX1262_REG_FREQ_15                               0x03E4      /**< freq 15 register */
#define SX1262_REG_DIOX_OUTPUT_ENABLE                    0x0580      /**< diox output enable register */
#define SX1262_REG_DIOX_INPUT_ENABLE                     0x0583      /**< diox input enable register */
#define SX1262_REG_DIOX_PULL_UP_CONTROL                  0x0584      /**< diox pull up control register */
#define SX1262_REG_DIOX_PULL_DOWN_CONTROL                0x0585      /**< diox pull down control register */
#define SX1262_REG_WHITENING_INIT_VALUE_MSB              0x06B8      /**< whitening initial value msb register */
#define SX1262_REG_WHITENING_INIT_VALUE_LSB              0x06B9      /**< whitening initial value lsb register */
#define SX1262_REG_CRC_INIT_VALUE_MSB                    0x06BC      /**< crc msb initial value register */
#define SX1262_REG_CRC_INIT_VALUE_LSB                    0x06BD      /**< crc lsb initial value register */
#define SX1262_REG_CRC_POLYNOMIAL_VALUE_MSB              0x06BE      /**< crc msb polynomial value register */
#define SX1262_REG_CRC_POLYNOMIAL_VALUE_LSB              0x06BF      /**< crc lsb polynomial value register */
#define SX1262_REG_SYNC_WORD_0                           0x06C0      /**< 1st byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_1                           0x06C1      /**< 2nd byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_2                           0x06C2      /**< 3rd byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_3                           0x06C3      /**< 4th byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_4                           0x06C4      /**< 5th byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_5                           0x06C5      /**< 6th byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_6                           0x06C6      /**< 7th byte of the sync word in fsk mode register */
#define SX1262_REG_SYNC_WORD_7                           0x06C7      /**< 8th byte of the sync word in fsk mode register */
#define SX1262_REG_NODE_ADDRESS                          0x06CD      /**< node address register */
#define SX1262_REG_BROADCAST_ADDRESS                     0x06CE      /**< broadcast address register */
#define SX1262_REG_IQ_POLARITY_SETUP                     0x0736      /**< iq polarity setup register */
#define SX1262_REG_LORA_SYNC_WORD_MSB                    0x0740      /**< lora sync word msb register */
#define SX1262_REG_LORA_SYNC_WORD_LSB                    0x0741      /**< lora sync word lsb register */
#define SX1262_REG_RANDOM_NUMBER_GEN_0                   0x0819      /**< random number gen 0 register */
#define SX1262_REG_RANDOM_NUMBER_GEN_1                   0x081A      /**< random number gen 1 register */
#define SX1262_REG_RANDOM_NUMBER_GEN_2                   0x081B      /**< random number gen 2 register */
#define SX1262_REG_RANDOM_NUMBER_GEN_3                   0x081C      /**< random number gen 3 register */
#define SX1262_REG_TX_MODULATION                         0x0889      /**< tx modulation register */
#define SX1262_REG_RX_GAIN                               0x08AC      /**< rx gain register */
#define SX1262_REG_TX_CLAMP_CONFIG                       0x08D8      /**< tx clamp config register */
#define SX1262_REG_OCP_CONFIGURATION                     0x08E7      /**< ocp configuration register */
#define SX1262_REG_RTC_CONTROL                           0x0902      /**< rtc control register */
#define SX1262_REG_XTA_TRIM                              0x0911      /**< xta trim register */
#define SX1262_REG_XTB_TRIM                              0x0912      /**< xtb trim register */
#define SX1262_REG_DIO3_OUTPUT_CONTROL                   0x0920      /**< dio3 output voltage control register */
#define SX1262_REG_EVENT_MASK                            0x0944      /**< event mask register */

/**
 * @brief      read bytes
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read failed
 * @note       none
 */
static uint8_t a_sx1262_spi_read(sx1262_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->spi_write_read(&reg, 1, buf, len) != 0)   /* spi read */
    {
        return 1;                                         /* return error */
    }
    else
    {
        return 0;                                         /* success return 0 */
    }
}

/**
 * @brief     write bytes
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 spi write failed
 *            - 2 len is over 383
 * @note      none
 */
static uint8_t a_sx1262_spi_write(sx1262_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (len > (384 - 1))                                              /* check the length */
    {
        handle->debug_print("sx1262: len is over 383.\n");            /* len is over 383 */
        
        return 2;                                                     /* return error */
    }
    
    memset(handle->buf, 0, sizeof(uint8_t) * 384);                    /* clear the buffer */
    handle->buf[0] = reg;                                             /* set the reg */
    memcpy(&handle->buf[1], buf, len);                                /* copy the buffer */
    if (handle->spi_write_read(handle->buf, len + 1, NULL, 0) != 0)   /* spi write */
    {
        return 1;                                                     /* return error */
    }
    else
    {
        return 0;                                                     /* success return 0 */
    }
}

/**
 * @brief      read register bytes
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read register failed
 *             - 2 len is over 383
 * @note       none
 */
static uint8_t a_sx1262_spi_read_register(sx1262_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t reg_buf[3];
    
    if (len > (384 - 1))                                                            /* check the length */
    {
        handle->debug_print("sx1262: len is over 383.\n");                          /* len is over 383 */
        
        return 2;                                                                   /* return error */
    }
    
    memset(handle->buf, 0, sizeof(uint8_t) * 384);                                  /* clear the buffer */
    reg_buf[0] = SX1262_COMMAND_READ_REGISTER;                                      /* set the command */
    reg_buf[1] = (reg >> 8) & 0xFF;                                                 /* set msb */
    reg_buf[2] = (reg >> 0) & 0xFF;                                                 /* set lsb */
    if (handle->spi_write_read((uint8_t *)reg_buf, 3, handle->buf, len + 1) != 0)   /* spi read */
    {
        return 1;                                                                   /* return error */
    }
    else
    {
        memcpy(buf, handle->buf + 1, len);                                          /* copy the data */
        
        return 0;                                                                   /* success return 0 */
    }
}

/**
 * @brief     write register bytes
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 spi write register failed
 *            - 2 len is over 381
 * @note      none
 */
static uint8_t a_sx1262_spi_write_register(sx1262_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    if (len > (384 - 3))                                              /* check the length */
    {
        handle->debug_print("sx1262: len is over 381.\n");            /* len is over 381 */
        
        return 2;                                                     /* return error */
    }
    
    memset(handle->buf, 0, sizeof(uint8_t) * 384);                    /* clear the buffer */
    handle->buf[0] = SX1262_COMMAND_WRITE_REGISTER;                   /* set the command */
    handle->buf[1] = (reg >> 8) & 0xFF;                               /* set reg msb */
    handle->buf[2] = (reg >> 0) & 0xFF;                               /* set reg lsb */
    memcpy(&handle->buf[3], buf, len);                                /* copy the buffer */
    if (handle->spi_write_read(handle->buf, len + 3, NULL, 0) != 0)   /* spi write */
    {
        return 1;                                                     /* return error */
    }
    else
    {
        return 0;                                                     /* success return 0 */
    }
}

/**
 * @brief     write buffer
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] offset buffer offset
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 spi write buffer failed
 *            - 2 len is over 382
 * @note      none
 */
static uint8_t a_sx1262_spi_write_buffer(sx1262_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len)
{
    if (len > (384 - 2))                                              /* check the length */
    {
        handle->debug_print("sx1262: len is over 382.\n");            /* len is over 382 */
        
        return 2;                                                     /* return error */
    }
    
    memset(handle->buf, 0, sizeof(uint8_t) * 384);                    /* clear the buffer */
    handle->buf[0] = SX1262_COMMAND_WRITE_BUFFER;                     /* set the command */
    handle->buf[1] = offset;                                          /* set reg msb */
    memcpy(&handle->buf[2], buf, len);                                /* copy the buffer */
    if (handle->spi_write_read(handle->buf, len + 2, NULL, 0) != 0)   /* spi write */
    {
        return 1;                                                     /* return error */
    }
    else
    {
        return 0;                                                     /* success return 0 */
    }
}

/**
 * @brief      read buffer
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  offset buffer offset
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read buffer failed
 *             - 2 len is over 383
 * @note      none
 */
static uint8_t a_sx1262_spi_read_buffer(sx1262_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len)
{
    uint8_t reg_buf[2];
    
    if (len > (384 - 1))                                                            /* check the length */
    {
        handle->debug_print("sx1262: len is over 383.\n");                          /* len is over 383 */
        
        return 2;                                                                   /* return error */
    }
    
    memset(handle->buf, 0, sizeof(uint8_t) * 384);                                  /* clear the buffer */
    reg_buf[0] = SX1262_COMMAND_READ_BUFFER ;                                       /* set the command */
    reg_buf[1] = offset;                                                            /* set msb */
    if (handle->spi_write_read((uint8_t *)reg_buf, 2, handle->buf, len + 1) != 0)   /* spi write */
    {
        return 1;                                                                   /* return error */
    }
    else
    {
        memcpy(buf, handle->buf + 1, len);                                          /* copy the data */
        
        return 0;                                                                   /* success return 0 */
    }
}

/**
 * @brief     check command busy
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 idle
 *            - 1 busy or error
 * @note      none
 */
static uint8_t a_sx1262_check_busy(sx1262_handle_t *handle)
{
    uint8_t level;
    uint8_t timeout;
    
    timeout = 100;                                            /* set max 100 */
    
    while (1)                                                 /* loop */
    {
        if (handle->busy_gpio_read((uint8_t *)&level) != 0)   /* read busy gpio */
        {
             return 1;                                        /* return error */
        }
        else
        {
            if (level == 0)                                   /* check level */
            {
                return 0;                                     /* success return 0 */
            }
            else if ((level == 1) && (timeout != 0))          /* check level and timeout */
            {
                handle->delay_ms(10);                         /* delay 10 ms */
                timeout--;                                    /* timeout-- */
                if (timeout != 0)                             /* check timeout */
                {
                    continue;                                 /* continue */
                }
                else
                {
                    return 1;                                 /* return error */
                }
            }
            else
            {
                return 1;                                     /* return error */
            }
        }
    }
}

/**
 * @brief     irq handler
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sx1262_irq_handler(sx1262_handle_t *handle)
{
    uint8_t res;
    uint8_t buf[3]; 
    uint16_t status;
    
    if (handle == NULL)                                                                                        /* check handle */
    {
        return 2;                                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                                   /* check handle initialization */
    {
        return 3;                                                                                              /* return error */
    }
    
    memset(buf, 0, sizeof(uint8_t) * 3);                                                                       /* clear the buffer */
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_IRQ_STATUS, (uint8_t *)buf, 3);                         /* read command */
    if (res != 0)                                                                                              /* check result */
    {
        handle->debug_print("sx1262: get irq status failed.\n");                                               /* get irq status failed */
       
        return 1;                                                                                              /* return error */
    }
    status = ((uint16_t)buf[1] << 8) | buf[2];                                                                 /* set status */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)&buf[1], 2);                  /* write command */
    if (res != 0)                                                                                              /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                                             /* clear irq status failed */
       
        return 1;                                                                                              /* return error */
    }
    
    handle->crc_error = 0;                                                                                     /* clear crc error */
    if ((status & SX1262_IRQ_PREAMBLE_DETECTED) != 0)                                                          /* if preamble detected */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_PREAMBLE_DETECTED, NULL, 0);                                   /* run callback */
        }
    }
    if ((status & SX1262_IRQ_SYNC_WORD_VALID) != 0)                                                            /* if valid sync word detected */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_SYNC_WORD_VALID, NULL, 0);                                     /* run callback */
        }
    }
    if ((status & SX1262_IRQ_HEADER_VALID) != 0)                                                               /* if valid header */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_HEADER_VALID, NULL, 0);                                        /* run callback */
        }
    }
    if ((status & SX1262_IRQ_HEADER_ERR) != 0)                                                                 /* if header error */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_HEADER_ERR, NULL, 0);                                          /* run callback */
        }
    }
    if ((status & SX1262_IRQ_CRC_ERR) != 0)                                                                    /* if crc error */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_CRC_ERR, NULL, 0);                                             /* run callback */
        }
        handle->crc_error = 1;                                                                                 /* set crc error */
    }
    if ((status & SX1262_IRQ_CAD_DONE) != 0)                                                                   /* if cad done */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_CAD_DONE, NULL, 0);                                            /* run callback */
        }
        handle->cad_done = 1;                                                                                  /* set cad done */
    }
    if ((status & SX1262_IRQ_CAD_DETECTED) != 0)                                                               /* if cad detected */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_CAD_DETECTED, NULL, 0);                                        /* run callback */
        }
        handle->cad_detected = 1;                                                                              /* set detected */
    }
    if ((status & SX1262_IRQ_TIMEOUT) != 0)                                                                    /* if timeout */
    {
        uint8_t control;
        uint8_t mask;
        
        control = 0x00;
        res = a_sx1262_spi_write_register(handle, SX1262_REG_DIO3_OUTPUT_CONTROL, (uint8_t *)&control, 1);     /* write register */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sx1262: write register failed.\n");                                           /* write register failed */
           
            return 1;                                                                                          /* return error */
        }
        
        /* read mask */
        res = a_sx1262_spi_read_register(handle, SX1262_REG_EVENT_MASK, (uint8_t *)&mask, 1);                  /* read register */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sx1262: read register failed.\n");                                            /* read register failed */
           
            return 1;                                                                                          /* return error */
        }
        mask |= 0x02;                                                                                          /* set mask */
        res = a_sx1262_spi_write_register(handle, SX1262_REG_EVENT_MASK, (uint8_t *)&mask, 1);                 /* write register */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sx1262: write register failed.\n");                                           /* write register failed */
           
            return 1;                                                                                          /* return error */
        }
        
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_TIMEOUT, NULL, 0);                                             /* run callback */
        }
        handle->timeout = 1;                                                                                   /* flag timeout */
    }
    if ((status & SX1262_IRQ_TX_DONE) != 0)                                                                    /* if tx done */
    {
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            handle->receive_callback(SX1262_IRQ_TX_DONE, NULL, 0);                                             /* run callback */
        }
        handle->tx_done = 1;                                                                                   /* flag tx done */
    }
    if ((status & SX1262_IRQ_RX_DONE) != 0)                                                                    /* if rx done */
    {
        uint8_t payload_length_rx;
        uint8_t rx_start_buffer_pointer;
        
        memset(buf, 0, sizeof(uint8_t) * 3);                                                                   /* clear the buffer */
        res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_RX_BUFFER_STATUS, (uint8_t *)buf, 3);               /* read command */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sx1262: get rx buffer status failed.\n");                                     /* get rx buffer status failed */
           
            return 1;                                                                                          /* return error */
        }
        payload_length_rx = buf[1];                                                                            /* set status */
        rx_start_buffer_pointer = buf[2];                                                                      /* set status */
        
        res = a_sx1262_spi_read_buffer(handle, rx_start_buffer_pointer, handle->receive_buf,
                                       payload_length_rx);                                                     /* read buffer */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sx1262: read buffer failed.\n");                                              /* read buffer failed */
           
            return 1;                                                                                          /* return error */
        }
        if (handle->receive_callback != NULL)                                                                  /* if receive callback */
        {
            if (handle->crc_error == 0)                                                                        /* check crc error */
            {
                handle->receive_callback(SX1262_IRQ_RX_DONE, handle->receive_buf, payload_length_rx);          /* run callback */
            }
            else
            {
                handle->receive_callback(SX1262_IRQ_RX_DONE, NULL, 0);                                         /* run callback */
            }
        }
    }
    
    return 0;                                                                                                  /* success return 0 */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 spi initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 reset gpio initialization failed
 *            - 5 busy gpio initialization failed
 *            - 6 reset chip failed
 * @note      none
 */
uint8_t sx1262_init(sx1262_handle_t *handle)
{
    uint8_t buf[1];
    uint8_t prev;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->debug_print == NULL)                                                       /* check debug_print */
    {
        return 3;                                                                          /* return error */
    }
    if (handle->spi_init == NULL)                                                          /* check spi_init */
    {
        handle->debug_print("sx1262: spi_init is null.\n");                                /* spi_init is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->spi_deinit == NULL)                                                        /* check spi_deinit */
    {
        handle->debug_print("sx1262: spi_deinit is null.\n");                              /* spi_deinit is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->spi_write_read == NULL)                                                    /* check spi_write_read */
    {
        handle->debug_print("sx1262: spi_write_read is null.\n");                          /* spi_write_read is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->reset_gpio_init == NULL)                                                   /* check reset_gpio_init */
    {
        handle->debug_print("sx1262: reset_gpio_init is null.\n");                         /* reset_gpio_init is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->reset_gpio_deinit == NULL)                                                 /* check reset_gpio_deinit */
    {
        handle->debug_print("sx1262: reset_gpio_deinit is null.\n");                       /* reset_gpio_deinit is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->reset_gpio_write == NULL)                                                  /* check reset_gpio_write */
    {
        handle->debug_print("sx1262: reset_gpio_write is null.\n");                        /* reset_gpio_write is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->busy_gpio_init == NULL)                                                    /* check busy_gpio_init */
    {
        handle->debug_print("sx1262: busy_gpio_init is null.\n");                          /* busy_gpio_init is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->busy_gpio_deinit == NULL)                                                  /* check busy_gpio_deinit */
    {
        handle->debug_print("sx1262: busy_gpio_deinit is null.\n");                        /* busy_gpio_deinit is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->busy_gpio_read == NULL)                                                    /* check busy_gpio_read */
    {
        handle->debug_print("sx1262: busy_gpio_read is null.\n");                          /* busy_gpio_read is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->delay_ms == NULL)                                                          /* check delay_ms */
    {
        handle->debug_print("sx1262: delay_ms is null.\n");                                /* delay_ms is null */
       
        return 3;                                                                          /* return error */
    }
    if (handle->receive_callback == NULL)                                                  /* check receive_callback */
    {
        handle->debug_print("sx1262: receive_callback is null.\n");                        /* receive_callback is null */
       
        return 3;                                                                          /* return error */
    }
    
    if (handle->spi_init() != 0)                                                           /* spi initialization */
    {
        handle->debug_print("sx1262: spi initialization failed.\n");                       /* spi initialization failed */
       
        return 1;                                                                          /* return error */ 
    }
    if (handle->reset_gpio_init() != 0)                                                    /* reset gpio initialization */
    {
        handle->debug_print("sx1262: reset gpio initialization failed.\n");                /* reset gpio initialization failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        
        return 4;                                                                          /* return error */ 
    }
    if (handle->busy_gpio_init() != 0)                                                     /* busy gpio initialization */
    {
        handle->debug_print("sx1262: busy gpio initialization failed.\n");                 /* busy gpio initialization failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        
        return 5;                                                                          /* return error */ 
    }
    
    if (handle->reset_gpio_write(1) != 0)                                                  /* set high */
    {
        handle->debug_print("sx1262: reset chip failed.\n");                               /* reset chip failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        (void)handle->busy_gpio_deinit();                                                  /* busy gpio deinit */
        
        return 6;                                                                          /* return error */ 
    }
    handle->delay_ms(5);                                                                   /* delay 5 ms */
    if (handle->reset_gpio_write(0) != 0)                                                  /* set low */
    {
        handle->debug_print("sx1262: reset chip failed.\n");                               /* reset chip failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        (void)handle->busy_gpio_deinit();                                                  /* busy gpio deinit */
        
        return 6;                                                                          /* return error */ 
    }
    handle->delay_ms(10);                                                                  /* delay 10 ms */
    if (handle->reset_gpio_write(1) != 0)                                                  /* set high */
    {
        handle->debug_print("sx1262: reset chip failed.\n");                               /* reset chip failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        (void)handle->busy_gpio_deinit();                                                  /* busy gpio deinit */
        
        return 6;                                                                          /* return error */ 
    }
    handle->delay_ms(5);                                                                   /* delay 5 ms */
    
    if (a_sx1262_spi_read(handle, SX1262_COMMAND_GET_STATUS, (uint8_t *)buf, 1) != 0)      /* read command */
    {
        handle->debug_print("sx1262: get status failed.\n");                               /* get status failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        (void)handle->busy_gpio_deinit();                                                  /* busy gpio deinit */
        
        return 6;                                                                          /* return error */
    }
    prev = 0x00;
    if (a_sx1262_spi_write(handle, SX1262_COMMAND_SET_STANDBY, (uint8_t *)&prev, 1) != 0)  /* write command */
    {
        handle->debug_print("sx1262: set standby failed.\n");                              /* set standby failed */
        (void)handle->spi_deinit();                                                        /* spi deinit */
        (void)handle->reset_gpio_deinit();                                                 /* reset gpio deinit */
        (void)handle->busy_gpio_deinit();                                                  /* busy gpio deinit */
        
        return 6;                                                                          /* return error */
    }
    handle->inited = 1;                                                                    /* flag finish initialization */
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 spi deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 *            - 5 power down failed
 *            - 6 busy gpio deinit failed
 *            - 7 reset gpio deinit failed
 * @note      none
 */
uint8_t sx1262_deinit(sx1262_handle_t *handle)
{
    uint8_t res, prev;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                     /* check busy */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                    /* chip is busy */
       
        return 4;                                                                          /* return error */
    }
    
    prev = 0x00;                                                                           /* set power down */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_SLEEP, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: power down failed.\n");                               /* power down failed */
       
        return 5;                                                                          /* return error */
    }
    
    res = handle->busy_gpio_deinit();                                                      /* busy gpio deinit */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: busy gpio deinit failed.\n");                         /* busy gpio deinit failed */
       
        return 6;                                                                          /* return error */
    }
    res = handle->reset_gpio_deinit();                                                     /* reset gpio deinit */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: reset gpio deinit failed.\n");                        /* reset gpio deinit failed */
       
        return 7;                                                                          /* return error */
    }
    res = handle->spi_deinit();                                                            /* spi deinit */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: spi deinit failed.\n");                               /* spi deinit failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     enter to the single receive mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] us rx timeout
 * @return    status code
 *            - 0 success
 *            - 1 lora single receive failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_single_receive(sx1262_handle_t *handle, double us)
{
    uint8_t res;
    uint8_t buf[3];
    uint16_t clear_irq_param;
    uint32_t timeout;
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    clear_irq_param = 0x03FF;                                                                   /* set mask */
    buf[0] = (clear_irq_param >> 8) & 0xFF;                                                     /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF;                                                     /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                              /* clear irq status failed */
       
        return 1;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    timeout = (uint32_t)(us / 15.625);                                                          /* convert real data to register data */
    buf[0] = (timeout >> 16) & 0xFF;                                                            /* bit 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF;                                                             /* bit 15 : 8 */
    buf[2] = (timeout >> 0) & 0xFF;                                                             /* bit 7 : 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RX, (uint8_t *)buf, 3);                 /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: set rx failed.\n");                                        /* set rx failed */
       
        return 1;                                                                               /* return error */
    }
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief     enter to the continuous receive mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 lora continuous receive failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_continuous_receive(sx1262_handle_t *handle)
{
    uint8_t res;
    uint16_t clear_irq_param;
    uint8_t buf[3];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    clear_irq_param = 0x03FF;                                                                   /* set mask */
    buf[0] = (clear_irq_param >> 8) & 0xFF;                                                     /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF;                                                     /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                              /* clear irq status failed */
       
        return 1;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    buf[0] = 0xFF;                                                                              /* bit 23 : 16 */
    buf[1] = 0xFF;                                                                              /* bit 15 : 8 */
    buf[2] = 0xFF;                                                                              /* bit 7 : 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RX, (uint8_t *)buf, 3);                 /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: set rx failed.\n");                                        /* set rx failed */
       
        return 1;                                                                               /* return error */
    }
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief      run the cad
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *enable pointer to an enable buffer
 * @return     status code
 *             - 0 success
 *             - 1 lora cad failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 *             - 5 cad timeout
 * @note       none
 */
uint8_t sx1262_lora_cad(sx1262_handle_t *handle, sx1262_bool_t *enable)
{
    uint8_t res;
    uint16_t clear_irq_param;
    uint8_t buf[2];
    uint16_t timeout;
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    clear_irq_param = 0x03FF;                                                                   /* set mask */
    buf[0] = (clear_irq_param >> 8) & 0xFF;                                                     /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF;                                                     /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                              /* clear irq status failed */
       
        return 1;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    handle->cad_done = 0;                                                                       /* clear cad done */
    handle->cad_detected = 0;                                                                   /* clear cad done */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_CAD, NULL, 0);                          /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: set cad failed.\n");                                       /* set cad failed */
       
        return 1;                                                                               /* return error */
    }
    
    timeout = 10000;                                                                            /*  set timeout */
    while ((timeout != 0) && (handle->cad_done == 0))                                           /* wait */
    {
        handle->delay_ms(1);                                                                    /* delay 1 ms */
        timeout--;                                                                              /* timeout-- */
    }
    if (timeout == 0)                                                                           /* check timeout */
    {
        handle->debug_print("sx1262: cad timeout.\n");                                          /* cad timeout */
       
        return 5;                                                                               /* return error */
    }
    if (handle->cad_detected == 1)                                                              /* set cad */
    {
        *enable = SX1262_BOOL_TRUE;                                                             /* enable */
    }
    else
    {
        *enable = SX1262_BOOL_FALSE;                                                            /* disable */
    }
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief      check the packet error
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *enable pointer to a bool buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_check_packet_error(sx1262_handle_t *handle, sx1262_bool_t *enable)
{
    if (handle == NULL)                                  /* check handle */
    {
        return 2;                                        /* return error */
    }
    if (handle->inited != 1)                             /* check handle initialization */
    {
        return 3;                                        /* return error */
    }
    
    *enable = (sx1262_bool_t)(handle->crc_error);        /* check error */
    
    return 0;                                            /* success return 0 */
}

/**
 * @brief     send the lora data
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] standby_src clock source
 * @param[in] preamble_length preamble length
 * @param[in] header_type header type
 * @param[in] crc_type crc type
 * @param[in] invert_iq_enable bool value
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @param[in] us timeout
 * @return    status code
 *            - 0 success
 *            - 1 send failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 *            - 5 send timeout
 *            - 6 irq timeout
 *            - 7 unknown result
 * @note      none
 */
uint8_t sx1262_lora_transmit(sx1262_handle_t *handle, sx1262_clock_source_t standby_src,
                             uint16_t preamble_length, sx1262_lora_header_t header_type,
                             sx1262_lora_crc_type_t crc_type, sx1262_bool_t invert_iq_enable,
                             uint8_t *buf, uint16_t len, uint32_t us)
{
    uint8_t res;
    uint8_t prev;
    uint8_t buffer[6];
    uint16_t clear_irq_param;
    uint32_t ms;
    uint32_t reg;
    
    if (handle == NULL)                                                                                    /* check handle */
    {
        return 2;                                                                                          /* return error */
    }
    if (handle->inited != 1)                                                                               /* check handle initialization */
    {
        return 3;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    clear_irq_param = 0x03FF;                                                                              /* set mask */
    buffer[0] = (clear_irq_param >> 8) & 0xFF;                                                             /* set param */
    buffer[1] = (clear_irq_param >> 0) & 0xFF;                                                             /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)buffer, 2);               /* write command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                                         /* clear irq status failed */
       
        return 1;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    prev = standby_src;                                                                                    /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_STANDBY, (uint8_t *)&prev, 1);                     /* write command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: set standby failed.\n");                                              /* set standby failed */
       
        return 1;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    buffer[0] = (preamble_length >> 8) & 0xFF;                                                             /* set param */
    buffer[1] = (preamble_length >> 0) & 0xFF;                                                             /* set param */
    buffer[2] = header_type;                                                                               /* set param */
    buffer[3] = (uint8_t)len;                                                                              /* set param */
    buffer[4] = crc_type;                                                                                  /* set param */
    buffer[5] = invert_iq_enable;                                                                          /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_PACKET_PARAMS, (uint8_t *)buffer, 6);              /* write command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: set lora modulation params failed.\n");                               /* set lora modulation params failed */
       
        return 1;                                                                                          /* return error */
    }
    
    if (invert_iq_enable == SX1262_BOOL_FALSE)                                                             /* not invert iq */
    {
        uint8_t setup;
        
        res = a_sx1262_check_busy(handle);                                                                 /* check busy */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
           
            return 4;                                                                                      /* return error */
        }
        
        res = a_sx1262_spi_read_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1);      /* read register */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
           
            return 1;                                                                                      /* return error */
        }
        
        setup |= (1 << 2);                                                                                 /* set bit 2 */
        res = a_sx1262_spi_write_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1);     /* read register */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
           
            return 1;                                                                                      /* return error */
        }
    }
    else
    {
        uint8_t setup;
        
        res = a_sx1262_check_busy(handle);                                                                 /* check busy */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
           
            return 4;                                                                                      /* return error */
        }
        
        res = a_sx1262_spi_read_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1);      /* read register */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
           
            return 1;                                                                                      /* return error */
        }
        
        setup &= ~(1 << 2);                                                                                /* clear bit 2 */
        res = a_sx1262_spi_write_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1);     /* read register */
        if (res != 0)                                                                                      /* check result */
        {
            handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
           
            return 1;                                                                                      /* return error */
        }
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    if (a_sx1262_spi_write_buffer(handle, 0x00, buf, len) != 0)                                            /* write buffer */
    {
        handle->debug_print("sx1262: write buffer failed.\n");                                             /* write buffer failed */
       
        return 1;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    reg = (uint32_t)(us / 15.625);                                                                         /* convert the timeout */
    buffer[0] = (reg >> 16) & 0xFF;                                                                        /* bit 23 : 16 */
    buffer[1] = (reg >> 8) & 0xFF;                                                                         /* bit 15 : 8 */
    buffer[2] = (reg >> 0) & 0xFF;                                                                         /* bit 7 : 0 */
    handle->tx_done = 0;                                                                                   /* flag 0 */
    handle->timeout = 0;                                                                                   /* flag 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_TX, (uint8_t *)buffer, 3);                         /* write command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: set tx failed.\n");                                                   /* set tx failed */
       
        return 1;                                                                                          /* return error */
    }
    ms = us / 1000 + 10000;                                                                                /* set timeout */
    while ((ms != 0) && (handle->tx_done == 0) && (handle->timeout == 0))                                  /* check timeout */
    {
        handle->delay_ms(1);                                                                               /* delay 1 ms */
        ms--;                                                                                              /* ms-- */
    }
    if ((ms != 0) && (handle->tx_done == 1))                                                               /* check the result */
    {
        return 0;                                                                                          /* success return 0 */
    }
    else if ((ms == 0) && (handle->tx_done == 0))                                                          /* check the result */
    {
        handle->debug_print("sx1262: send timeout.\n");                                                    /* send timeout */
       
        return 5;                                                                                          /* return error */
    }
    else if (handle->timeout == 1)                                                                         /* check the result */
    {
        handle->debug_print("sx1262: irq timeout.\n");                                                     /* irq timeout */
       
        return 6;                                                                                          /* return error */
    }
    else
    {
        handle->debug_print("sx1262: unknown result.\n");                                                  /* unknown result */
       
        return 7;                                                                                          /* return error */
    }
}

/**
 * @brief     write the register
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] reg register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write register failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_write_register(sx1262_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                  /* check busy */
    if (res != 0)                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                 /* chip is busy */
       
        return 4;                                                       /* return error */
    }
    
    if (a_sx1262_spi_write_register(handle, reg, buf, len) != 0)        /* write register */
    {
        handle->debug_print("sx1262: write register failed.\n");        /* write register failed */
       
        return 1;                                                       /* return error */
    }
    
    return 0;                                                           /* success return 0 */
}

/**
 * @brief      read the register
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read register failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_read_register(sx1262_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                /* check handle */
    {
        return 2;                                                      /* return error */
    }
    if (handle->inited != 1)                                           /* check handle initialization */
    {
        return 3;                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                 /* check busy */
    if (res != 0)                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                /* chip is busy */
       
        return 4;                                                      /* return error */
    }
    
    if (a_sx1262_spi_read_register(handle, reg, buf, len) != 0)        /* read register */
    {
        handle->debug_print("sx1262: read register failed.\n");        /* read register failed */
       
        return 1;                                                      /* return error */
    }
    
    return 0;                                                          /* success return 0 */
}

/**
 * @brief     write the buffer
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] offset buffer offset
 * @param[in] *buf pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write buffer failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_write_buffer(sx1262_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                               /* check handle */
    {
        return 2;                                                     /* return error */
    }
    if (handle->inited != 1)                                          /* check handle initialization */
    {
        return 3;                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                /* check busy */
    if (res != 0)                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");               /* chip is busy */
       
        return 4;                                                     /* return error */
    }
    
    if (a_sx1262_spi_write_buffer(handle, offset, buf, len) != 0)     /* write buffer */
    {
        handle->debug_print("sx1262: write buffer failed.\n");        /* write buffer failed */
       
        return 1;                                                     /* return error */
    }
    
    return 0;                                                         /* success return 0 */
}

/**
 * @brief      read the buffer
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  offset buffer offset
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read buffer failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_read_buffer(sx1262_handle_t *handle, uint8_t offset, uint8_t *buf, uint16_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                              /* check handle */
    {
        return 2;                                                    /* return error */
    }
    if (handle->inited != 1)                                         /* check handle initialization */
    {
        return 3;                                                    /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                               /* check busy */
    if (res != 0)                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");              /* chip is busy */
       
        return 4;                                                    /* return error */
    }
    
    if (a_sx1262_spi_read_buffer(handle, offset, buf, len) != 0)     /* read buffer */
    {
        handle->debug_print("sx1262: read buffer failed.\n");        /* read buffer failed */
       
        return 1;                                                    /* return error */
    }
    
    return 0;                                                        /* success return 0 */
}

/**
 * @brief     enter to the sleep mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] mode start mode
 * @param[in] rtc_wake_up_enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set sleep failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_sleep(sx1262_handle_t *handle, sx1262_start_mode_t mode, sx1262_bool_t rtc_wake_up_enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                     /* check busy */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                    /* chip is busy */
       
        return 4;                                                                          /* return error */
    }
    
    prev = (uint8_t)((mode << 2) | (rtc_wake_up_enable << 0));                             /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_SLEEP, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: set sleep failed.\n");                                /* set sleep failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     enter to the standby mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] src clock source
 * @return    status code
 *            - 0 success
 *            - 1 set standby failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_standby(sx1262_handle_t *handle, sx1262_clock_source_t src)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                     /* check busy */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                    /* chip is busy */
       
        return 4;                                                                          /* return error */
    }
    
    prev = src;                                                                            /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_STANDBY, (uint8_t *)&prev, 1);     /* write command */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: set standby failed.\n");                              /* set standby failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief     enter to the frequency synthesis mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set frequency synthesis failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_frequency_synthesis(sx1262_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                     /* check handle */
    {
        return 2;                                                           /* return error */
    }
    if (handle->inited != 1)                                                /* check handle initialization */
    {
        return 3;                                                           /* return error */
    }

    res = a_sx1262_check_busy(handle);                                      /* check busy */
    if (res != 0)                                                           /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                     /* chip is busy */
       
        return 4;                                                           /* return error */
    }
    
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_FS, NULL, 0);       /* write command */
    if (res != 0)                                                           /* check result */
    {
        handle->debug_print("sx1262: set fs failed.\n");                    /* set fs failed */
       
        return 1;                                                           /* return error */
    }
    
    return 0;                                                               /* success return 0 */
}

/**
 * @brief     enter to the tx mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] timeout tx timeout
 * @return    status code
 *            - 0 success
 *            - 1 set tx failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      0x000000 means timeout disable, tx single mode
 */
uint8_t sx1262_set_tx(sx1262_handle_t *handle, uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[3];
    
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }

    res = a_sx1262_check_busy(handle);                                              /* check busy */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                             /* chip is busy */
       
        return 4;                                                                   /* return error */
    }
    
    buf[0] = (timeout >> 16) & 0xFF;                                                /* bit 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF;                                                 /* bit 15 : 8 */
    buf[2] = (timeout >> 0) & 0xFF;                                                 /* bit 7 : 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_TX, (uint8_t *)buf, 3);     /* write command */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("sx1262: set tx failed.\n");                            /* set tx failed */
       
        return 1;                                                                   /* return error */
    }
    
    return 0;                                                                       /* success return 0 */
}

/**
 * @brief     enter to the rx mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] timeout rx timeout
 * @return    status code
 *            - 0 success
 *            - 1 set rx failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      0x000000 means timeout disable, rx single mode
 *            0xFFFFFF means rx continuous mode
 */
uint8_t sx1262_set_rx(sx1262_handle_t *handle, uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[3];
    
    if (handle == NULL)                                                             /* check handle */
    {
        return 2;                                                                   /* return error */
    }
    if (handle->inited != 1)                                                        /* check handle initialization */
    {
        return 3;                                                                   /* return error */
    }

    res = a_sx1262_check_busy(handle);                                              /* check busy */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                             /* chip is busy */
       
        return 4;                                                                   /* return error */
    }
    
    buf[0] = (timeout >> 16) & 0xFF;                                                /* bit 23 : 16 */
    buf[1] = (timeout >> 8) & 0xFF;                                                 /* bit 15 : 8 */
    buf[2] = (timeout >> 0) & 0xFF;                                                 /* bit 7 : 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RX, (uint8_t *)buf, 3);     /* write command */
    if (res != 0)                                                                   /* check result */
    {
        handle->debug_print("sx1262: set rx failed.\n");                            /* set rx failed */
       
        return 1;                                                                   /* return error */
    }
    
    return 0;                                                                       /* success return 0 */
}

/**
 * @brief      convert the timeout to the register raw data
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  us timeout
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_timeout_convert_to_register(sx1262_handle_t *handle, double us, uint32_t *reg)
{
    if (handle == NULL)                    /* check handle */
    {
        return 2;                          /* return error */
    }
    if (handle->inited != 1)               /* check handle initialization */
    {
        return 3;                          /* return error */
    }
    
    *reg = (uint32_t)(us / 15.625);        /* convert real data to register data */
    
    return 0;                              /* success return 0 */
}

/**
 * @brief      convert the register raw data to the timeout
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register raw data
 * @param[out] *us pointer to a us buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_timeout_convert_to_data(sx1262_handle_t *handle, uint32_t reg, double *us)
{
    if (handle == NULL)                  /* check handle */
    {
        return 2;                        /* return error */
    }
    if (handle->inited != 1)             /* check handle initialization */
    {
        return 3;                        /* return error */
    }
    
    *us = (double)(reg) * 15.625;        /* convert raw data to real data */
    
    return 0;                            /* success return 0 */
}

/**
 * @brief     stop timer on preamble
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set stop timer on preamble failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_stop_timer_on_preamble(sx1262_handle_t *handle, sx1262_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    prev = enable;                                                                                      /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_STOP_TIMER_ON_PREAMBLE, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: set stop timer on preamble failed.\n");                            /* set stop timer on preamble failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief     set the rx duty cycle
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] rx_period rx period
 * @param[in] sleep_period sleep period
 * @return    status code
 *            - 0 success
 *            - 1 set rx duty cycle failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      t_preamble + t_header <= 2 * rx_period + sleep_period
 */
uint8_t sx1262_set_rx_duty_cycle(sx1262_handle_t *handle, uint32_t rx_period, uint32_t sleep_period)
{
    uint8_t res;
    uint8_t buf[6];
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    buf[0] = (rx_period >> 16) & 0xFF;                                                         /* bit 23 : 16 */
    buf[1] = (rx_period >> 8) & 0xFF;                                                          /* bit 15 : 8 */
    buf[2] = (rx_period >> 0) & 0xFF;                                                          /* bit 7 : 0 */
    buf[3] = (sleep_period >> 16) & 0xFF;                                                      /* bit 23 : 16 */
    buf[4] = (sleep_period >> 8) & 0xFF;                                                       /* bit 15 : 8 */
    buf[5] = (sleep_period >> 0) & 0xFF;                                                       /* bit 7 : 0 */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RX_DUTY_CYCLE, (uint8_t *)buf, 6);     /* write command */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: set rx duty cycle failed.\n");                            /* set rx duty cycle failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief     run the cad
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set cad failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_cad(sx1262_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }

    res = a_sx1262_check_busy(handle);                                       /* check busy */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                      /* chip is busy */
       
        return 4;                                                            /* return error */
    }
    
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_CAD, NULL, 0);       /* write command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("sx1262: set cad failed.\n");                    /* set cad failed */
       
        return 1;                                                            /* return error */
    }
    
    return 0;                                                                /* success return 0 */
}

/**
 * @brief     enter to the tx continuous wave mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set tx continuous wave failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_tx_continuous_wave(sx1262_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_TX_CONTINUOUS_WAVE, NULL, 0);        /* write command */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: set tx continuous wave failed.\n");                     /* set tx continuous wave failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     enter to the tx infinite preamble mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set tx infinite preamble failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_tx_infinite_preamble(sx1262_handle_t *handle)
{
    uint8_t res;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_TX_INFINITE_PREAMBLE, NULL, 0);        /* write command */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: set tx infinite preamble failed.\n");                     /* set tx infinite preamble failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief     set the regulator_mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] mode regulator mode
 * @return    status code
 *            - 0 success
 *            - 1 set regulator mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_regulator_mode(sx1262_handle_t *handle, sx1262_regulator_mode_t mode)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                             /* check handle */
    {
        return 2;                                                                                   /* return error */
    }
    if (handle->inited != 1)                                                                        /* check handle initialization */
    {
        return 3;                                                                                   /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                              /* check busy */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                             /* chip is busy */
       
        return 4;                                                                                   /* return error */
    }
    
    prev = mode;                                                                                    /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_REGULATOR_MODE, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: set regulator mode failed.\n");                                /* set regulator mode failed */
       
        return 1;                                                                                   /* return error */
    }
    
    return 0;                                                                                       /* success return 0 */
}

/**
 * @brief     set the calibration settings
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] settings calibration param
 * @return    status code
 *            - 0 success
 *            - 1 set calibration failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_calibration(sx1262_handle_t *handle, uint8_t settings)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    prev = settings;                                                                           /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_CALIBRATE, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: set calibration failed.\n");                              /* set calibration failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief     set the calibration image frequency
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] freq1 image frequency 1
 * @param[in] freq2 image frequency 2
 * @return    status code
 *            - 0 success
 *            - 1 set calibration image failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_calibration_image(sx1262_handle_t *handle, uint8_t freq1, uint8_t freq2)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                            /* check handle */
    {
        return 2;                                                                                  /* return error */
    }
    if (handle->inited != 1)                                                                       /* check handle initialization */
    {
        return 3;                                                                                  /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                             /* check busy */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                            /* chip is busy */
       
        return 4;                                                                                  /* return error */
    }
    
    buf[0] = freq1;                                                                                /* set param */
    buf[1] = freq2;                                                                                /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_CALIBRATE_IMAGE, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: set calibration image failed.\n");                            /* set calibration image failed */
       
        return 1;                                                                                  /* return error */
    }
    
    return 0;                                                                                      /* success return 0 */
}

/**
 * @brief     set the pa config
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] pa_duty_cycle pa duty cycle
 * @param[in] hp_max max power
 * @return    status code
 *            - 0 success
 *            - 1 set the pa config failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_pa_config(sx1262_handle_t *handle, uint8_t pa_duty_cycle, uint8_t hp_max)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    buf[0] = pa_duty_cycle;                                                                  /* set param */
    buf[1] = hp_max;                                                                         /* set param */
    buf[2] = 0x00;                                                                           /* set param */
    buf[3] = 0x01;                                                                           /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_PA_CONFIG, (uint8_t *)buf, 4);       /* write command */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: set pa config failed.\n");                              /* set pa config failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     set the rx tx fallback mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] mode rx tx fallback mode
 * @return    status code
 *            - 0 success
 *            - 1 set rx tx fallback mode failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_rx_tx_fallback_mode(sx1262_handle_t *handle, sx1262_rx_tx_fallback_mode_t mode)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                                  /* check handle */
    {
        return 2;                                                                                        /* return error */
    }
    if (handle->inited != 1)                                                                             /* check handle initialization */
    {
        return 3;                                                                                        /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                                   /* check busy */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                  /* chip is busy */
       
        return 4;                                                                                        /* return error */
    }
    
    prev = mode;                                                                                         /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RX_TX_FALLBACK_MODE, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: set rx tx fallback mode failed.\n");                                /* set rx tx fallback mode failed */
       
        return 1;                                                                                        /* return error */
    }
    
    return 0;                                                                                            /* success return 0 */
}

/**
 * @brief     set the dio irq params
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] irq_mask irq mask
 * @param[in] dio1_mask dio1 mask
 * @param[in] dio2_mask dio2 mask
 * @param[in] dio3_mask dio3 mask
 * @return    status code
 *            - 0 success
 *            - 1 set dio irq params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio_irq_params(sx1262_handle_t *handle, uint16_t irq_mask, uint16_t dio1_mask,
                                  uint16_t dio2_mask, uint16_t dio3_mask)
{
    uint8_t res;
    uint8_t buf[8];
    
    if (handle == NULL)                                                                           /* check handle */
    {
        return 2;                                                                                 /* return error */
    }
    if (handle->inited != 1)                                                                      /* check handle initialization */
    {
        return 3;                                                                                 /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                            /* check busy */
    if (res != 0)                                                                                 /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                           /* chip is busy */
       
        return 4;                                                                                 /* return error */
    }
    
    buf[0] = (irq_mask >> 8) & 0xFF;                                                              /* set param */
    buf[1] = (irq_mask >> 0) & 0xFF;                                                              /* set param */
    buf[2] = (dio1_mask >> 8) & 0xFF;                                                             /* set param */
    buf[3] = (dio1_mask >> 0) & 0xFF;                                                             /* set param */
    buf[4] = (dio2_mask >> 8) & 0xFF;                                                             /* set param */
    buf[5] = (dio2_mask >> 0) & 0xFF;                                                             /* set param */
    buf[6] = (dio3_mask >> 8) & 0xFF;                                                             /* set param */
    buf[7] = (dio3_mask >> 0) & 0xFF;                                                             /* set param */
    
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_DIO_IRQ_PARAMS, (uint8_t *)buf, 8);       /* write command */
    if (res != 0)                                                                                 /* check result */
    {
        handle->debug_print("sx1262: set dio irq params failed.\n");                              /* set dio irq params failed */
       
        return 1;                                                                                 /* return error */
    }
    
    return 0;                                                                                     /* success return 0 */
}

/**
 * @brief      get the irq status
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *status pointer to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get irq status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_irq_status(sx1262_handle_t *handle, uint16_t *status)
{
    uint8_t res;
    uint8_t buf[3];
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_IRQ_STATUS, (uint8_t *)buf, 3);       /* read command */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: get irq status failed.\n");                             /* get irq status failed */
       
        return 1;                                                                            /* return error */
    }
    *status = ((uint16_t)buf[1] << 8) | buf[2];                                              /* set status */
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     clear the irq status
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] clear_irq_param irq mask
 * @return    status code
 *            - 0 success
 *            - 1 clear irq status failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_clear_irq_status(sx1262_handle_t *handle, uint16_t clear_irq_param)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    buf[0] = (clear_irq_param >> 8) & 0xFF;                                                     /* set param */
    buf[1] = (clear_irq_param >> 0) & 0xFF;                                                     /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_IRQ_STATUS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: clear irq status failed.\n");                              /* clear irq status failed */
       
        return 1;                                                                               /* return error */
    }
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief     set dio2 as rf switch ctrl
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set dio2 as rf switch ctrl failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio2_as_rf_switch_ctrl(sx1262_handle_t *handle, sx1262_bool_t enable)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                                     /* check handle */
    {
        return 2;                                                                                           /* return error */
    }
    if (handle->inited != 1)                                                                                /* check handle initialization */
    {
        return 3;                                                                                           /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                                      /* check busy */
    if (res != 0)                                                                                           /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                     /* chip is busy */
       
        return 4;                                                                                           /* return error */
    }
    
    prev = enable;                                                                                          /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_DIO2_AS_RF_SWITCH_CTRL, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                           /* check result */
    {
        handle->debug_print("sx1262: set dio2 as rf switch ctrl failed.\n");                                /* set dio2 as rf switch ctrl failed */
       
        return 1;                                                                                           /* return error */
    }
    
    return 0;                                                                                               /* success return 0 */
}

/**
 * @brief     set dio3 as tcxo ctrl
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] voltage tcxo voltage
 * @param[in] delay tcxo ctrl delay
 * @return    status code
 *            - 0 success
 *            - 1 set dio3 as tcxo ctrl failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio3_as_tcxo_ctrl(sx1262_handle_t *handle, sx1262_tcxo_voltage_t voltage, uint32_t delay)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    buf[0] = voltage;                                                                                /* set param */
    buf[1] = (delay >> 16) & 0xFF;                                                                   /* set param */
    buf[2] = (delay >> 8) & 0xFF;                                                                    /* set param */
    buf[3] = (delay >> 0) & 0xFF;                                                                    /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_DIO3_AS_TCXO_CTRL, (uint8_t *)buf, 4);       /* write command */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: set dio3 as tcxo ctrl status failed.\n");                       /* set dio3 as tcxo ctrl status failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief      convert the frequency to the register raw data
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  freq frequency
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_frequency_convert_to_register(sx1262_handle_t *handle, uint32_t freq, uint32_t *reg)
{
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }
    
    *reg = (uint32_t)(powf(2.0f, 25.0f) / (32 * powf(10.f, 6.0f)) * freq);        /* convert real data to register data */
    
    return 0;                                                                     /* success return 0 */
}

/**
 * @brief      convert the register raw data to the frequency
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register raw data
 * @param[out] *freq pointer to a frequency buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_frequency_convert_to_data(sx1262_handle_t *handle, uint32_t reg, uint32_t *freq)
{
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }
    
    *freq = (uint32_t)(32 * powf(10.f, 6.0f) / powf(2.0f, 25.0f) * reg);          /* convert real data to register data */
    
    return 0;                                                                     /* success return 0 */
}

/**
 * @brief     set the rf frequency
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] reg rf frequency register data
 * @return    status code
 *            - 0 success
 *            - 1 set rf frequency failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_rf_frequency(sx1262_handle_t *handle, uint32_t reg)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    buf[0] = (reg >> 24) & 0xFF;                                                                /* set param */
    buf[1] = (reg >> 16) & 0xFF;                                                                /* set param */
    buf[2] = (reg >> 8) & 0xFF;                                                                 /* set param */
    buf[3] = (reg >> 0) & 0xFF;                                                                 /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_RF_FREQUENCY, (uint8_t *)buf, 4);       /* write command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: set rf frequency failed.\n");                              /* set rf frequency failed */
       
        return 1;                                                                               /* return error */
    }
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief     set the packet type
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] type packet type
 * @return    status code
 *            - 0 success
 *            - 1 set packet type failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_packet_type(sx1262_handle_t *handle, sx1262_packet_type_t type)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                           /* check busy */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                          /* chip is busy */
       
        return 4;                                                                                /* return error */
    }
    
    prev = type;                                                                                 /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_PACKET_TYPE, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: set packet type failed.\n");                                /* set packet type failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

/**
 * @brief      get the packet type
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *type pointer to a packet type buffer
 * @return     status code
 *             - 0 success
 *             - 1 get packet type failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_packet_type(sx1262_handle_t *handle, sx1262_packet_type_t *type)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                       /* check handle */
    {
        return 2;                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                  /* check handle initialization */
    {
        return 3;                                                                             /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                        /* check busy */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                       /* chip is busy */
       
        return 4;                                                                             /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_PACKET_TYPE, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: get packet type failed.\n");                             /* get packet type failed */
       
        return 1;                                                                             /* return error */
    }
    *type = (sx1262_packet_type_t)(buf[1]);                                                   /* get type */
    
    return 0;                                                                                 /* success return 0 */
}

/**
 * @brief     set the tx params
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] dbm rf power 
 * @param[in] t ramp time
 * @return    status code
 *            - 0 success
 *            - 1 set tx params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_tx_params(sx1262_handle_t *handle, int8_t dbm, sx1262_ramp_time_t t)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    buf[0] = dbm;                                                                            /* set param */
    buf[1] = t;                                                                              /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_TX_PARAMS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: set tx params failed.\n");                              /* set tx params failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     set the modulation params in GFSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] br bit rate
 * @param[in] shape pulse shape
 * @param[in] bw bandwidth
 * @param[in] fdev frequency deviation
 * @return    status code
 *            - 0 success
 *            - 1 set gfsk modulation params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_gfsk_modulation_params(sx1262_handle_t *handle, uint32_t br, sx1262_gfsk_pulse_shape_t shape, 
                                          sx1262_gfsk_bandwidth_t bw, uint32_t fdev)
{
    uint8_t res;
    uint8_t buf[8];
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    buf[0] = (br >> 16) & 0xFF;                                                                      /* set param */
    buf[1] = (br >> 8) & 0xFF;                                                                       /* set param */
    buf[2] = (br >> 0) & 0xFF;                                                                       /* set param */
    buf[3] = shape;                                                                                  /* set param */
    buf[4] = bw;                                                                                     /* set param */
    buf[5] = (fdev >> 16) & 0xFF;                                                                    /* set param */
    buf[6] = (fdev >> 8) & 0xFF;                                                                     /* set param */
    buf[7] = (fdev >> 0) & 0xFF;                                                                     /* set param */ 
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_MODULATION_PARAMS, (uint8_t *)buf, 8);       /* write command */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: set gfsk modulation params failed.\n");                         /* set gfsk modulation params failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief      convert the bit rate to the register raw data
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  br bit rate
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_gfsk_bit_rate_convert_to_register(sx1262_handle_t *handle, uint32_t br, uint32_t *reg)
{
    if (handle == NULL)                                             /* check handle */
    {
        return 2;                                                   /* return error */
    }
    if (handle->inited != 1)                                        /* check handle initialization */
    {
        return 3;                                                   /* return error */
    }
    
    *reg = (uint32_t)(32 * (32 * powf(10.f, 6.0f))) / br;           /* convert real data to register data */
    
    return 0;                                                       /* success return 0 */
}

/**
 * @brief      convert the register raw data to the bit rate
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register raw data
 * @param[out] *br pointer to a bit rate buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_gfsk_bit_rate_convert_to_data(sx1262_handle_t *handle, uint32_t reg, uint32_t *br)
{
    if (handle == NULL)                                          /* check handle */
    {
        return 2;                                                /* return error */
    }
    if (handle->inited != 1)                                     /* check handle initialization */
    {
        return 3;                                                /* return error */
    }
    
    *br = (uint32_t)(32 * 32 * powf(10.f, 6.0f) / reg);          /* convert real data to register data */
    
    return 0;                                                    /* success return 0 */
}

/**
 * @brief      convert the frequency deviation to the register raw data
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  freq frequency deviation
 * @param[out] *reg pointer to a register raw buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_gfsk_frequency_deviation_convert_to_register(sx1262_handle_t *handle, uint32_t freq, uint32_t *reg)
{
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }
    
    *reg = (uint32_t)(powf(2.0f, 25.0f) * freq / (32 * powf(10.f, 6.0f)));        /* convert real data to register data */
    
    return 0;                                                                     /* success return 0 */
}

/**
 * @brief      convert the register raw data to the frequency deviation
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  reg register raw data
 * @param[out] *freq pointer to a frequency deviation buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_gfsk_frequency_deviation_convert_to_data(sx1262_handle_t *handle, uint32_t reg, uint32_t *freq)
{
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }
    
    *freq = (uint32_t)(32 * powf(10.f, 6.0f) / powf(2.0f, 25.0f) * reg);          /* convert real data to register data */
    
    return 0;                                                                     /* success return 0 */
}

/**
 * @brief     set the modulation params in LoRa mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] sf spreading factor
 * @param[in] bw bandwidth
 * @param[in] cr coding rate
 * @param[in] low_data_rate_optimize_enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set lora modulation params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_lora_modulation_params(sx1262_handle_t *handle, sx1262_lora_sf_t sf, sx1262_lora_bandwidth_t bw, 
                                          sx1262_lora_cr_t cr, sx1262_bool_t low_data_rate_optimize_enable)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }

    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    buf[0] = sf;                                                                                     /* set param */
    buf[1] = bw;                                                                                     /* set param */
    buf[2] = cr;                                                                                     /* set param */
    buf[3] = low_data_rate_optimize_enable;                                                          /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_MODULATION_PARAMS, (uint8_t *)buf, 4);       /* write command */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: set lora modulation params failed.\n");                         /* set lora modulation params failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief     set the packet params in GFSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] preamble_length preamble length
 * @param[in] detector_length preamble detector length
 * @param[in] sync_word_length sync word length
 * @param[in] filter address filter
 * @param[in] packet_type packet type
 * @param[in] payload_length length of the payload
 * @param[in] crc_type crc type
 * @param[in] whitening_enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set gfsk packet params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 *            - 5 sync word length is over 0x40
 * @note      none
 */
uint8_t sx1262_set_gfsk_packet_params(sx1262_handle_t *handle, uint16_t preamble_length,
                                      sx1262_gfsk_preamble_detector_length_t detector_length,
                                      uint8_t sync_word_length, sx1262_gfsk_addr_filter_t filter,
                                      sx1262_gfsk_packet_type_t packet_type, uint8_t payload_length,
                                      sx1262_gfsk_crc_type_t crc_type, sx1262_bool_t whitening_enable)
{
    uint8_t res;
    uint8_t buf[9];
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }
    if (sync_word_length > 0x40)                                                                     /* check sync word length */
    {
        handle->debug_print("sx1262: sync word length is over 0x40.\n");                             /* chip is busy */
       
        return 5;                                                                                    /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    buf[0] = (preamble_length >> 8) & 0xFF;                                                          /* set param */
    buf[1] = (preamble_length >> 0) & 0xFF;                                                          /* set param */
    buf[2] = detector_length;                                                                        /* set param */
    buf[3] = sync_word_length;                                                                       /* set param */
    buf[4] = filter;                                                                                 /* set param */
    buf[5] = packet_type;                                                                            /* set param */
    buf[6] = payload_length;                                                                         /* set param */
    buf[7] = crc_type;                                                                               /* set param */
    buf[8] = whitening_enable;                                                                       /* set param */ 
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_PACKET_PARAMS, (uint8_t *)buf, 9);           /* write command */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: set gfsk modulation params failed.\n");                         /* set gfsk modulation params failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief     set the packet params in LoRa mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] preamble_length preamble length
 * @param[in] header_type header type
 * @param[in] payload_length length of payload
 * @param[in] crc_type crc type
 * @param[in] invert_iq_enable bool value
 * @return    status code
 *            - 0 success
 *            - 1 set lora packet params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_lora_packet_params(sx1262_handle_t *handle, uint16_t preamble_length,
                                      sx1262_lora_header_t header_type, uint8_t payload_length,
                                      sx1262_lora_crc_type_t crc_type, sx1262_bool_t invert_iq_enable)
{
    uint8_t res;
    uint8_t buf[6];
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                           /* check busy */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                          /* chip is busy */
       
        return 4;                                                                                /* return error */
    }
    
    buf[0] = (preamble_length >> 8) & 0xFF;                                                      /* set param */
    buf[1] = (preamble_length >> 0) & 0xFF;                                                      /* set param */
    buf[2] = header_type;                                                                        /* set param */
    buf[3] = payload_length;                                                                     /* set param */
    buf[4] = crc_type;                                                                           /* set param */
    buf[5] = invert_iq_enable;                                                                   /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_PACKET_PARAMS, (uint8_t *)buf, 6);       /* write command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: set lora modulation params failed.\n");                     /* set lora modulation params failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

/**
 * @brief     set the cad params
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] num cad symbol number
 * @param[in] cad_det_peak cad detection peak
 * @param[in] cad_det_min min cad detection peak 
 * @param[in] mode cad exit mode
 * @param[in] timeout cad timeout
 * @return    status code
 *            - 0 success
 *            - 1 set cad params failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_cad_params(sx1262_handle_t *handle, sx1262_lora_cad_symbol_num_t num,
                              uint8_t cad_det_peak, uint8_t cad_det_min, sx1262_lora_cad_exit_mode_t mode,
                              uint32_t timeout)
{
    uint8_t res;
    uint8_t buf[7];
    
    if (handle == NULL)                                                                       /* check handle */
    {
        return 2;                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                  /* check handle initialization */
    {
        return 3;                                                                             /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                        /* check busy */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                       /* chip is busy */
       
        return 4;                                                                             /* return error */
    }
    
    buf[0] = num;                                                                             /* set param */
    buf[1] = cad_det_peak;                                                                    /* set param */
    buf[2] = cad_det_min;                                                                     /* set param */
    buf[3] = mode;                                                                            /* set param */
    buf[4] = (timeout >> 16) & 0xFF;                                                          /* set param */
    buf[5] = (timeout >> 8) & 0xFF;                                                           /* set param */
    buf[6] = (timeout >> 0) & 0xFF;                                                           /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_CAD_PARAMS, (uint8_t *)buf, 7);       /* write command */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: set cad params failed.\n");                              /* set cad params failed */
       
        return 1;                                                                             /* return error */
    }
    
    return 0;                                                                                 /* success return 0 */
}

/**
 * @brief     set the buffer base address
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] tx_base_addr tx base address
 * @param[in] rx_base_addr rx base address
 * @return    status code
 *            - 0 success
 *            - 1 set buffer base address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_buffer_base_address(sx1262_handle_t *handle, uint8_t tx_base_addr, uint8_t rx_base_addr)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    buf[0] = tx_base_addr;                                                                             /* set param */
    buf[1] = rx_base_addr;                                                                             /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_BUFFER_BASE_ADDRESS, (uint8_t *)buf, 2);       /* write command */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: set buffer base address failed.\n");                              /* set buffer base address failed */
       
        return 1;                                                                                      /* return error */
    }
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief     set the lora symbol number timeout
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] symb_num symbol number
 * @return    status code
 *            - 0 success
 *            - 1 set lora symb num timeout failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_lora_symb_num_timeout(sx1262_handle_t *handle, uint8_t symb_num)
{
    uint8_t res;
    uint8_t prev;
    
    if (handle == NULL)                                                                                    /* check handle */
    {
        return 2;                                                                                          /* return error */
    }
    if (handle->inited != 1)                                                                               /* check handle initialization */
    {
        return 3;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    prev = symb_num;                                                                                       /* set param */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_SET_LORA_SYMB_NUM_TIMEOUT, (uint8_t *)&prev, 1);       /* write command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: set lora symb num timeout failed.\n");                                /* set lora symb num timeout failed */
       
        return 1;                                                                                          /* return error */
    }
    
    return 0;                                                                                              /* success return 0 */
}

/**
 * @brief      get the status
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *status pointer to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_status(sx1262_handle_t *handle, uint8_t *status)
{
    uint8_t res;
    uint8_t buf[1];
    
    if (handle == NULL)                                                                  /* check handle */
    {
        return 2;                                                                        /* return error */
    }
    if (handle->inited != 1)                                                             /* check handle initialization */
    {
        return 3;                                                                        /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                   /* check busy */
    if (res != 0)                                                                        /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                  /* chip is busy */
       
        return 4;                                                                        /* return error */
    }
    
    memset(buf, 0, sizeof(uint8_t) * 1);                                                 /* clear the buffer */
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_STATUS, (uint8_t *)buf, 1);       /* read command */
    if (res != 0)                                                                        /* check result */
    {
        handle->debug_print("sx1262: get status failed.\n");                             /* get status failed */
       
        return 1;                                                                        /* return error */
    }
    *status = buf[0];                                                                    /* set status */
    
    return 0;                                                                            /* success return 0 */
}

/**
 * @brief      get the rx buffer status
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *payload_length_rx pointer to an rx payload length buffer
 * @param[out] *rx_start_buffer_pointer pointer to an rx start pointer buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rx buffer status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_rx_buffer_status(sx1262_handle_t *handle, uint8_t *payload_length_rx, uint8_t *rx_start_buffer_pointer)
{
    uint8_t res;
    uint8_t buf[3];
    
    if (handle == NULL)                                                                            /* check handle */
    {
        return 2;                                                                                  /* return error */
    }
    if (handle->inited != 1)                                                                       /* check handle initialization */
    {
        return 3;                                                                                  /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                             /* check busy */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                            /* chip is busy */
       
        return 4;                                                                                  /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_RX_BUFFER_STATUS, (uint8_t *)buf, 3);       /* read command */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: get rx buffer status failed.\n");                             /* get rx buffer status failed */
       
        return 1;                                                                                  /* return error */
    }
    *payload_length_rx = buf[1];                                                                   /* set status */
    *rx_start_buffer_pointer = buf[2];                                                             /* set status */
    
    return 0;                                                                                      /* success return 0 */
}

/**
 * @brief      get the packet status in GFSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *rx_status pointer to an rx status buffer
 * @param[out] *rssi_sync_raw pointer to an rssi sync raw buffer
 * @param[out] *rssi_avg_raw pointer to an rssi avg raw buffer
 * @param[out] *rssi_sync pointer to an rssi sync buffer
 * @param[out] *rssi_avg pointer to an rssi avg buffer
 * @return     status code
 *             - 0 success
 *             - 1 get gfsk packet status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_gfsk_packet_status(sx1262_handle_t *handle, uint8_t *rx_status, uint8_t *rssi_sync_raw,
                                      uint8_t *rssi_avg_raw, float *rssi_sync, float *rssi_avg)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_PACKET_STATUS, (uint8_t *)buf, 4);       /* read command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: get packet status failed.\n");                             /* get packet status failed */
       
        return 1;                                                                               /* return error */
    }
    *rx_status= buf[1];                                                                         /* set status */
    *rssi_sync_raw = buf[2];                                                                    /* set status */
    *rssi_avg_raw = buf[3];                                                                     /* set status */
    *rssi_sync = -(float)(*rssi_sync_raw) / 2.0f;                                               /* set status */
    *rssi_avg = -(float)(*rssi_avg_raw) / 2.0f;                                                 /* set status */
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief      get the packet status in LoRa mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *rssi_pkt_raw pointer to an rssi packet raw buffer
 * @param[out] *snr_pkt_raw pointer to an snr packet raw buffer
 * @param[out] *signal_rssi_pkt_raw pointer to a signal rssi packet raw buffer
 * @param[out] *rssi_pkt pointer to an rssi packet buffer
 * @param[out] *snr_pkt pointer to an snr packet buffer
 * @param[out] *signal_rssi_pkt pointer to a signal rssi packet buffer
 * @return     status code
 *             - 0 success
 *             - 1 get lora packet status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_lora_packet_status(sx1262_handle_t *handle, uint8_t *rssi_pkt_raw, uint8_t *snr_pkt_raw,
                                      uint8_t *signal_rssi_pkt_raw, float *rssi_pkt, float *snr_pkt, float *signal_rssi_pkt)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_PACKET_STATUS, (uint8_t *)buf, 4);       /* read command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: get packet status failed.\n");                             /* get packet status failed */
       
        return 1;                                                                               /* return error */
    }
    *rssi_pkt_raw= buf[1];                                                                      /* set status */
    *snr_pkt_raw = buf[2];                                                                      /* set status */
    *signal_rssi_pkt_raw = buf[3];                                                              /* set status */
    *rssi_pkt = -(float)(*rssi_pkt_raw) / 2.0f;                                                 /* set status */
    *snr_pkt = (float)(*snr_pkt_raw) / 4.0f;                                                    /* set status */
    *signal_rssi_pkt = -(float)(*signal_rssi_pkt_raw) / 2.0f;                                   /* set status */
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief      get the instantaneous rssi
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *rssi_inst_raw pointer to an rssi instantaneous raw buffer
 * @param[out] *rssi_inst pointer to an rssi instantaneous buffer
 * @return     status code
 *             - 0 success
 *             - 1 get instantaneous rssi failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_instantaneous_rssi(sx1262_handle_t *handle, uint8_t *rssi_inst_raw, float *rssi_inst)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                     /* check handle */
    {
        return 2;                                                                           /* return error */
    }
    if (handle->inited != 1)                                                                /* check handle initialization */
    {
        return 3;                                                                           /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                      /* check busy */
    if (res != 0)                                                                           /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                     /* chip is busy */
       
        return 4;                                                                           /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_RSSI_LNST, (uint8_t *)buf, 2);       /* read command */
    if (res != 0)                                                                           /* check result */
    {
        handle->debug_print("sx1262: get instantaneous rssi failed.\n");                    /* get instantaneous rssi failed */
       
        return 1;                                                                           /* return error */
    }
    *rssi_inst_raw = buf[1];                                                                /* set status */
    *rssi_inst = -(float)(*rssi_inst_raw) / 2.0f;                                           /* set status */
    
    return 0;                                                                               /* success return 0 */
}

/**
 * @brief      get the stats
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *pkt_received pointer to a packet received counter buffer
 * @param[out] *pkt_crc_error pointer to a packet crc error buffer
 * @param[out] *pkt_length_header_error pointer to a packet length header error buffer
 * @return     status code
 *             - 0 success
 *             - 1 get stats failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_stats(sx1262_handle_t *handle, uint16_t *pkt_received, uint16_t *pkt_crc_error, uint16_t *pkt_length_header_error)
{
    uint8_t res;
    uint8_t buf[7];
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                  /* check busy */
    if (res != 0)                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                 /* chip is busy */
       
        return 4;                                                                       /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_STATS, (uint8_t *)buf, 7);       /* read command */
    if (res != 0)                                                                       /* check result */
    {
        handle->debug_print("sx1262: get stats failed.\n");                             /* get stats failed */
       
        return 1;                                                                       /* return error */
    }
    *pkt_received = (uint16_t)(((uint16_t)buf[1] << 8) | buf[2]);                       /* set status */
    *pkt_crc_error = (uint16_t)(((uint16_t)buf[3] << 8) | buf[4]);                      /* set status */
    *pkt_length_header_error = (uint16_t)(((uint16_t)buf[5] << 8) | buf[6]);            /* set status */
    
    return 0;                                                                           /* success return 0 */
}

/**
 * @brief     reset the stats
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] pkt_received received packet counter mask
 * @param[in] pkt_crc_error packet crc error mask
 * @param[in] pkt_length_header_error packet length header error mask
 * @return    status code
 *            - 0 success
 *            - 1 reset stats failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_reset_stats(sx1262_handle_t *handle, uint16_t pkt_received, uint16_t pkt_crc_error, uint16_t pkt_length_header_error)
{
    uint8_t res;
    uint8_t buf[6];
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                     /* check busy */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                    /* chip is busy */
       
        return 4;                                                                          /* return error */
    }
    
    buf[0] = (pkt_received >> 8) & 0xFF;                                                   /* set status */
    buf[1] = (pkt_received >> 0) & 0xFF;                                                   /* set status */
    buf[2] = (pkt_crc_error >> 8) & 0xFF;                                                  /* set status */
    buf[3] = (pkt_crc_error >> 0) & 0xFF;                                                  /* set status */
    buf[4] = (pkt_length_header_error >> 8) & 0xFF;                                        /* set status */
    buf[5] = (pkt_length_header_error >> 0) & 0xFF;                                        /* set status */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_RESET_STATS, (uint8_t *)buf, 6);       /* write command */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("sx1262: reset stats failed.\n");                              /* reset stats failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

/**
 * @brief      get the device errors
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *op_error pointer to an op error buffer
 * @return     status code
 *             - 0 success
 *             - 1 get device errors failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_device_errors(sx1262_handle_t *handle, uint16_t *op_error)
{
    uint8_t res;
    uint8_t buf[3];
    
    if (handle == NULL)                                                                         /* check handle */
    {
        return 2;                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                    /* check handle initialization */
    {
        return 3;                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                          /* check busy */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                         /* chip is busy */
       
        return 4;                                                                               /* return error */
    }
    
    res = a_sx1262_spi_read(handle, SX1262_COMMAND_GET_DEVICE_ERRORS, (uint8_t *)buf, 3);       /* read command */
    if (res != 0)                                                                               /* check result */
    {
        handle->debug_print("sx1262: get device errors failed.\n");                             /* get device errors failed */
       
        return 1;                                                                               /* return error */
    }
    *op_error = (uint16_t)(((uint16_t)buf[1] << 8) | buf[2]);                                   /* set status */
    
    return 0;                                                                                   /* success return 0 */
}

/**
 * @brief     clear the device errors
 * @param[in] *handle pointer to an sx1262 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 clear device errors failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_clear_device_errors(sx1262_handle_t *handle)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                             /* check handle */
    {
        return 2;                                                                                   /* return error */
    }
    if (handle->inited != 1)                                                                        /* check handle initialization */
    {
        return 3;                                                                                   /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                              /* check busy */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                             /* chip is busy */
       
        return 4;                                                                                   /* return error */
    }
    
    buf[0] = 0x00;                                                                                  /* set status */
    buf[1] = 0x00;                                                                                  /* set status */
    res = a_sx1262_spi_write(handle, SX1262_COMMAND_CLEAR_DEVICE_ERRORS, (uint8_t *)buf, 2);        /* write command */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: clear device errors failed.\n");                               /* clear device errors failed */
       
        return 1;                                                                                   /* return error */
    }
    
    return 0;                                                                                       /* success return 0 */
}

/**
 * @brief     set the whitening initial value in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk whitening initial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_whitening_initial_value(sx1262_handle_t *handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                      /* check handle */
    {
        return 2;                                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                                 /* check handle initialization */
    {
        return 3;                                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                       /* check busy */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                      /* chip is busy */
       
        return 4;                                                                                            /* return error */
    }
    
    buf[0] = (value >> 8) & 0xFF;                                                                            /* set msb */
    buf[1] = (value >> 0) & 0xFF;                                                                            /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_WHITENING_INIT_VALUE_MSB, (uint8_t *)buf, 2);       /* write register */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                             /* write register failed */
       
        return 1;                                                                                            /* return error */
    }
    
    return 0;                                                                                                /* success return 0 */
}

/**
 * @brief      get the whitening initial value in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk whitening initial value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_whitening_initial_value(sx1262_handle_t *handle, uint16_t *value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                      /* check handle */
    {
        return 2;                                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                                 /* check handle initialization */
    {
        return 3;                                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                       /* check busy */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                      /* chip is busy */
       
        return 4;                                                                                            /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_WHITENING_INIT_VALUE_MSB, (uint8_t *)buf, 2);        /* read register */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                              /* read register failed */
       
        return 1;                                                                                            /* return error */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);                                                     /* set value */
    
    return 0;                                                                                                /* success return 0 */
}

/**
 * @brief     set the crc initial value in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk crc initial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_crc_initical_value(sx1262_handle_t *handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    buf[0] = (value >> 8) & 0xFF;                                                                      /* set msb */
    buf[1] = (value >> 0) & 0xFF;                                                                      /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_CRC_INIT_VALUE_MSB, (uint8_t *)buf, 2);       /* write register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                       /* write register failed */
       
        return 1;                                                                                      /* return error */
    }
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief      get the crc initical value in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk crc initical value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_crc_initical_value(sx1262_handle_t *handle, uint16_t *value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_CRC_INIT_VALUE_MSB, (uint8_t *)buf, 2);        /* read register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
       
        return 1;                                                                                      /* return error */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);                                               /* set value */
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief     set the crc polynomial value in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] value set value
 * @return    status code
 *            - 0 success
 *            - 1 set fsk crc polynomial value failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_crc_polynomial_value(sx1262_handle_t *handle, uint16_t value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                      /* check handle */
    {
        return 2;                                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                                 /* check handle initialization */
    {
        return 3;                                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                       /* check busy */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                      /* chip is busy */
       
        return 4;                                                                                            /* return error */
    }
    
    buf[0] = (value >> 8) & 0xFF;                                                                            /* set msb */
    buf[1] = (value >> 0) & 0xFF;                                                                            /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_CRC_POLYNOMIAL_VALUE_MSB, (uint8_t *)buf, 2);       /* write register */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                             /* write register failed */
       
        return 1;                                                                                            /* return error */
    }
    
    return 0;                                                                                                /* success return 0 */
}

/**
 * @brief      get the crc polynomial value in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk crc polynomial value failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_crc_polynomial_value(sx1262_handle_t *handle, uint16_t *value)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                      /* check handle */
    {
        return 2;                                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                                 /* check handle initialization */
    {
        return 3;                                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                       /* check busy */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                      /* chip is busy */
       
        return 4;                                                                                            /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_CRC_POLYNOMIAL_VALUE_MSB, (uint8_t *)buf, 2);        /* read register */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                              /* read register failed */
       
        return 1;                                                                                            /* return error */
    }
    *value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);                                                     /* set value */
    
    return 0;                                                                                                /* success return 0 */
}

/**
 * @brief     set the sync word in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] *sync_word pointer to sync word buffer
 * @return    status code
 *            - 0 success
 *            - 1 set fsk sync word failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_sync_word(sx1262_handle_t *handle, uint8_t sync_word[8])
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_SYNC_WORD_0, (uint8_t *)sync_word, 8);       /* write register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                      /* write register failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief      get the sync word in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *sync_word pointer to sync word buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk sync word failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_sync_word(sx1262_handle_t *handle, uint8_t sync_word[8])
{
    uint8_t res;
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_SYNC_WORD_0, (uint8_t *)sync_word, 8);       /* read register */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                      /* read register failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief     set the node address in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] addr node address
 * @return    status code
 *            - 0 success
 *            - 1 set fsk node address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_node_address(sx1262_handle_t *handle, uint8_t addr)
{
    uint8_t res;
    
    if (handle == NULL)                                                                            /* check handle */
    {
        return 2;                                                                                  /* return error */
    }
    if (handle->inited != 1)                                                                       /* check handle initialization */
    {
        return 3;                                                                                  /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                             /* check busy */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                            /* chip is busy */
       
        return 4;                                                                                  /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_NODE_ADDRESS, (uint8_t *)&addr, 1);       /* write register */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                   /* write register failed */
       
        return 1;                                                                                  /* return error */
    }
    
    return 0;                                                                                      /* success return 0 */
}

/**
 * @brief      get the node address in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *addr pointer to a node address buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk node address failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_node_address(sx1262_handle_t *handle, uint8_t *addr)
{
    uint8_t res;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                           /* check busy */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                          /* chip is busy */
       
        return 4;                                                                                /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_NODE_ADDRESS, (uint8_t *)addr, 1);       /* read register */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                  /* read register failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

/**
 * @brief     set the broadcast address in FSK mode
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] addr broadcast address
 * @return    status code
 *            - 0 success
 *            - 1 set fsk broadcast address failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fsk_broadcast_address(sx1262_handle_t *handle, uint8_t addr)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_BROADCAST_ADDRESS, (uint8_t *)&addr, 1);       /* write register */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                        /* write register failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief      get the broadcast address in FSK mode
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *addr pointer to a broadcast address buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fsk broadcast address failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fsk_broadcast_address(sx1262_handle_t *handle, uint8_t *addr)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_BROADCAST_ADDRESS, (uint8_t *)addr, 1);       /* read register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                       /* read register failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief     set the iq polarity
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] setup settings
 * @return    status code
 *            - 0 success
 *            - 1 set iq polarity failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_iq_polarity(sx1262_handle_t *handle, uint8_t setup)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                  /* check handle */
    {
        return 2;                                                                                        /* return error */
    }
    if (handle->inited != 1)                                                                             /* check handle initialization */
    {
        return 3;                                                                                        /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                   /* check busy */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                  /* chip is busy */
       
        return 4;                                                                                        /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)&setup, 1);       /* write register */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                         /* write register failed */
       
        return 1;                                                                                        /* return error */
    }
    
    return 0;                                                                                            /* success return 0 */
}

/**
 * @brief      get the iq polarity
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *setup pointer to a settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get iq polarity failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_iq_polarity(sx1262_handle_t *handle, uint8_t *setup)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_IQ_POLARITY_SETUP, (uint8_t *)setup, 1);       /* read register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
       
        return 1;                                                                                      /* return error */
    }
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief     set the lora sync word
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] sync_word sync word
 * @return    status code
 *            - 0 success
 *            - 1 set lora sync word failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_lora_sync_word(sx1262_handle_t *handle, uint16_t sync_word)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    buf[0] = (sync_word >> 8) & 0xFF;                                                                  /* set msb */
    buf[1] = (sync_word >> 0) & 0xFF;                                                                  /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_LORA_SYNC_WORD_MSB, (uint8_t *)buf, 2);       /* write register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                       /* write register failed */
       
        return 1;                                                                                      /* return error */
    }
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief      get the lora sync word
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *sync_word pointer to an sync word buffer
 * @return     status code
 *             - 0 success
 *             - 1 get lora sync word failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_lora_sync_word(sx1262_handle_t *handle, uint16_t *sync_word)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_LORA_SYNC_WORD_MSB, (uint8_t *)buf, 2);       /* read register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                       /* read register failed */
       
        return 1;                                                                                     /* return error */
    }
    *sync_word = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);                                          /* set value */
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief      get the random number
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *r pointer to a random number buffer
 * @return     status code
 *             - 0 success
 *             - 1 get random number failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_random_number(sx1262_handle_t *handle, uint32_t *r)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_RANDOM_NUMBER_GEN_0, (uint8_t *)buf, 4);       /* read register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                        /* read register failed */
       
        return 1;                                                                                      /* return error */
    }
    *r = (uint32_t)((uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 | 
                    (uint32_t)buf[2] << 8 | buf[3]);                                                   /* set rand */
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief     set the tx modulation
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] modulation tx modulation
 * @return    status code
 *            - 0 success
 *            - 1 set tx modulation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_tx_modulation(sx1262_handle_t *handle, uint8_t modulation)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                   /* check handle */
    {
        return 2;                                                                                         /* return error */
    }
    if (handle->inited != 1)                                                                              /* check handle initialization */
    {
        return 3;                                                                                         /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                    /* check busy */
    if (res != 0)                                                                                         /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                   /* chip is busy */
       
        return 4;                                                                                         /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_TX_MODULATION, (uint8_t *)&modulation, 1);       /* write register */
    if (res != 0)                                                                                         /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                          /* write register failed */
       
        return 1;                                                                                         /* return error */
    }
    
    return 0;                                                                                             /* success return 0 */
}

/**
 * @brief      get the tx modulation
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *modulation pointer to a tx modulation buffer
 * @return     status code
 *             - 0 success
 *             - 1 get tx modulation failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_tx_modulation(sx1262_handle_t *handle, uint8_t *modulation)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_TX_MODULATION, (uint8_t *)modulation, 1);       /* read register */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                         /* read register failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief     set the rx gain
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] gain rx gain
 * @return    status code
 *            - 0 success
 *            - 1 set rx gain failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_rx_gain(sx1262_handle_t *handle, uint8_t gain)
{
    uint8_t res;
    
    if (handle == NULL)                                                                       /* check handle */
    {
        return 2;                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                  /* check handle initialization */
    {
        return 3;                                                                             /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                        /* check busy */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                       /* chip is busy */
       
        return 4;                                                                             /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_RX_GAIN, (uint8_t *)&gain, 1);       /* write register */
    if (res != 0)                                                                             /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                              /* write register failed */
       
        return 1;                                                                             /* return error */
    }
    
    return 0;                                                                                 /* success return 0 */
}

/**
 * @brief      get the rx gain
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *gain pointer to an rx gain buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rx gain failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_rx_gain(sx1262_handle_t *handle, uint8_t *gain)
{
    uint8_t res;
    
    if (handle == NULL)                                                                     /* check handle */
    {
        return 2;                                                                           /* return error */
    }
    if (handle->inited != 1)                                                                /* check handle initialization */
    {
        return 3;                                                                           /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                      /* check busy */
    if (res != 0)                                                                           /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                     /* chip is busy */
       
        return 4;                                                                           /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_RX_GAIN, (uint8_t *)gain, 1);       /* read register */
    if (res != 0)                                                                           /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                             /* read register failed */
       
        return 1;                                                                           /* return error */
    }
    
    return 0;                                                                               /* success return 0 */
}

/**
 * @brief     set the tx clamp config
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] config tx clamp config
 * @return    status code
 *            - 0 success
 *            - 1 set tx clamp config failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_tx_clamp_config(sx1262_handle_t *handle, uint8_t config)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_TX_CLAMP_CONFIG, (uint8_t *)&config, 1);       /* write register */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                        /* write register failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief      get the tx clamp config
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *config pointer to a tx clamp config buffer
 * @return     status code
 *             - 0 success
 *             - 1 get tx clamp config failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_tx_clamp_config(sx1262_handle_t *handle, uint8_t *config)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_TX_CLAMP_CONFIG, (uint8_t *)config, 1);       /* read register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                       /* read register failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief     set the ocp
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] ocp over current protection level
 * @return    status code
 *            - 0 success
 *            - 1 set ocp failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_ocp(sx1262_handle_t *handle, uint8_t ocp)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                 /* check busy */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                /* chip is busy */
       
        return 4;                                                                                      /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_OCP_CONFIGURATION, (uint8_t *)&ocp, 1);       /* write register */
    if (res != 0)                                                                                      /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                       /* write register failed */
       
        return 1;                                                                                      /* return error */
    }
    
    return 0;                                                                                          /* success return 0 */
}

/**
 * @brief      get the ocp
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *ocp pointer to an over current protection level buffer
 * @return     status code
 *             - 0 success
 *             - 1 get ocp failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_ocp(sx1262_handle_t *handle, uint8_t *ocp)
{
    uint8_t res;
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_OCP_CONFIGURATION, (uint8_t *)ocp, 1);       /* read register */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                      /* read register failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief     set the rtc control
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] control rtc control
 * @return    status code
 *            - 0 success
 *            - 1 set rtc control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_rtc_control(sx1262_handle_t *handle, uint8_t control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                               /* check busy */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                              /* chip is busy */
       
        return 4;                                                                                    /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_RTC_CONTROL, (uint8_t *)&control, 1);       /* write register */
    if (res != 0)                                                                                    /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                     /* write register failed */
       
        return 1;                                                                                    /* return error */
    }
    
    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief      get the rtc control
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *control pointer to an rtc control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get rtc control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_rtc_control(sx1262_handle_t *handle, uint8_t *control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                            /* check handle */
    {
        return 2;                                                                                  /* return error */
    }
    if (handle->inited != 1)                                                                       /* check handle initialization */
    {
        return 3;                                                                                  /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                             /* check busy */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                            /* chip is busy */
       
        return 4;                                                                                  /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_RTC_CONTROL, (uint8_t *)control, 1);       /* read register */
    if (res != 0)                                                                                  /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                    /* read register failed */
       
        return 1;                                                                                  /* return error */
    }
    
    return 0;                                                                                      /* success return 0 */
}

/**
 * @brief     set the xta trim
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] trim xta trim
 * @return    status code
 *            - 0 success
 *            - 1 set xta trim failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_xta_trim(sx1262_handle_t *handle, uint8_t trim)
{
    uint8_t res;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_XTA_TRIM, (uint8_t *)&trim, 1);       /* write register */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                               /* write register failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      get the xta trim
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *trim pointer to an xta trim buffer
 * @return     status code
 *             - 0 success
 *             - 1 get xta trim failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_xta_trim(sx1262_handle_t *handle, uint8_t *trim)
{
    uint8_t res;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_XTA_TRIM, (uint8_t *)trim, 1);       /* read register */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                              /* read register failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     set the xtb trim
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] trim xtb trim
 * @return    status code
 *            - 0 success
 *            - 1 set xtb trim failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_xtb_trim(sx1262_handle_t *handle, uint8_t trim)
{
    uint8_t res;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_XTB_TRIM, (uint8_t *)&trim, 1);       /* write register */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                               /* write register failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief      get the xtb trim
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *trim pointer to an xtb trim buffer
 * @return     status code
 *             - 0 success
 *             - 1 get xtb trim failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_xtb_trim(sx1262_handle_t *handle, uint8_t *trim)
{
    uint8_t res;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                       /* check busy */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                      /* chip is busy */
       
        return 4;                                                                            /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_XTB_TRIM, (uint8_t *)trim, 1);       /* read register */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                              /* read register failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

/**
 * @brief     set the dio3 output
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] control dio3 output control
 * @return    status code
 *            - 0 success
 *            - 1 set dio3 output control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio3_output_control(sx1262_handle_t *handle, uint8_t control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                      /* check handle */
    {
        return 2;                                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                                 /* check handle initialization */
    {
        return 3;                                                                                            /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                       /* check busy */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                      /* chip is busy */
       
        return 4;                                                                                            /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_DIO3_OUTPUT_CONTROL, (uint8_t *)&control, 1);       /* write register */
    if (res != 0)                                                                                            /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                             /* write register failed */
       
        return 1;                                                                                            /* return error */
    }
    
    return 0;                                                                                                /* success return 0 */
}

/**
 * @brief      get the dio3 output
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *control pointer to a dio3 output control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio3 output control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_dio3_output_control(sx1262_handle_t *handle, uint8_t *control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                    /* check handle */
    {
        return 2;                                                                                          /* return error */
    }
    if (handle->inited != 1)                                                                               /* check handle initialization */
    {
        return 3;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_DIO3_OUTPUT_CONTROL, (uint8_t *)control, 1);       /* read register */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                            /* read register failed */
       
        return 1;                                                                                          /* return error */
    }
    
    return 0;                                                                                              /* success return 0 */
}

/**
 * @brief     set the event mask
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] mask event mask
 * @return    status code
 *            - 0 success
 *            - 1 set event mask failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_event_mask(sx1262_handle_t *handle, uint8_t mask)
{
    uint8_t res;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                           /* check busy */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                          /* chip is busy */
       
        return 4;                                                                                /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_EVENT_MASK, (uint8_t *)&mask, 1);       /* write register */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                 /* write register failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

/**
 * @brief      get the event mask
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *mask pointer to an event mask buffer
 * @return     status code
 *             - 0 success
 *             - 1 get event mask failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_event_mask(sx1262_handle_t *handle, uint8_t *mask)
{
    uint8_t res;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                         /* check busy */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                        /* chip is busy */
       
        return 4;                                                                              /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_EVENT_MASK, (uint8_t *)mask, 1);       /* read register */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                /* read register failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

/**
 * @brief     set the dio output enable
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] enable enable settings
 * @return    status code
 *            - 0 success
 *            - 1 set dio output enable failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio_output_enable(sx1262_handle_t *handle, uint8_t enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                    /* check handle */
    {
        return 2;                                                                                          /* return error */
    }
    if (handle->inited != 1)                                                                               /* check handle initialization */
    {
        return 3;                                                                                          /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                     /* check busy */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                    /* chip is busy */
       
        return 4;                                                                                          /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_DIOX_OUTPUT_ENABLE, (uint8_t *)&enable, 1);       /* write register */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                           /* write register failed */
       
        return 1;                                                                                          /* return error */
    }
    
    return 0;                                                                                              /* success return 0 */
}

/**
 * @brief      get the dio output enable
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *enable pointer to an enable settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio output enable failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_dio_output_enable(sx1262_handle_t *handle, uint8_t *enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                  /* check handle */
    {
        return 2;                                                                                        /* return error */
    }
    if (handle->inited != 1)                                                                             /* check handle initialization */
    {
        return 3;                                                                                        /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                   /* check busy */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                  /* chip is busy */
       
        return 4;                                                                                        /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_DIOX_OUTPUT_ENABLE, (uint8_t *)enable, 1);       /* read register */
    if (res != 0)                                                                                        /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                          /* read register failed */
       
        return 1;                                                                                        /* return error */
    }
    
    return 0;                                                                                            /* success return 0 */
}

/**
 * @brief     set the dio input enable
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] enable enable settings
 * @return    status code
 *            - 0 success
 *            - 1 set dio input enable failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_dio_input_enable(sx1262_handle_t *handle, uint8_t enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                   /* check handle */
    {
        return 2;                                                                                         /* return error */
    }
    if (handle->inited != 1)                                                                              /* check handle initialization */
    {
        return 3;                                                                                         /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                    /* check busy */
    if (res != 0)                                                                                         /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                   /* chip is busy */
       
        return 4;                                                                                         /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_DIOX_INPUT_ENABLE, (uint8_t *)&enable, 1);       /* write register */
    if (res != 0)                                                                                         /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                          /* write register failed */
       
        return 1;                                                                                         /* return error */
    }
    
    return 0;                                                                                             /* success return 0 */
}

/**
 * @brief      get the dio input enable
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *enable pointer to an enable settings buffer
 * @return     status code
 *             - 0 success
 *             - 1 get dio input enable failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_dio_input_enable(sx1262_handle_t *handle, uint8_t *enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_DIOX_INPUT_ENABLE, (uint8_t *)enable, 1);       /* read register */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                         /* read register failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief     set the pull up control
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] control pull up control
 * @return    status code
 *            - 0 success
 *            - 1 set pull up control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_pull_up_control(sx1262_handle_t *handle, uint8_t control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                       /* check handle */
    {
        return 2;                                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                                  /* check handle initialization */
    {
        return 3;                                                                                             /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                        /* check busy */
    if (res != 0)                                                                                             /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                       /* chip is busy */
       
        return 4;                                                                                             /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_DIOX_PULL_UP_CONTROL, (uint8_t *)&control, 1);       /* write register */
    if (res != 0)                                                                                             /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                              /* write register failed */
       
        return 1;                                                                                             /* return error */
    }
    
    return 0;                                                                                                 /* success return 0 */
}

/**
 * @brief      get the pull up control
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *control pointer to a pull up control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pull up control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_pull_up_control(sx1262_handle_t *handle, uint8_t *control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                     /* check handle */
    {
        return 2;                                                                                           /* return error */
    }
    if (handle->inited != 1)                                                                                /* check handle initialization */
    {
        return 3;                                                                                           /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                      /* check busy */
    if (res != 0)                                                                                           /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                     /* chip is busy */
       
        return 4;                                                                                           /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_DIOX_PULL_UP_CONTROL, (uint8_t *)control, 1);       /* read register */
    if (res != 0)                                                                                           /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                             /* read register failed */
       
        return 1;                                                                                           /* return error */
    }
    
    return 0;                                                                                               /* success return 0 */
}

/**
 * @brief     set the pull down control
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] control pull down control
 * @return    status code
 *            - 0 success
 *            - 1 set pull down control failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_pull_down_control(sx1262_handle_t *handle, uint8_t control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                         /* check handle */
    {
        return 2;                                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                                    /* check handle initialization */
    {
        return 3;                                                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                          /* check busy */
    if (res != 0)                                                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                         /* chip is busy */
       
        return 4;                                                                                               /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_DIOX_PULL_DOWN_CONTROL, (uint8_t *)&control, 1);       /* write register */
    if (res != 0)                                                                                               /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                                /* write register failed */
       
        return 1;                                                                                               /* return error */
    }
    
    return 0;                                                                                                   /* success return 0 */
}

/**
 * @brief      get the pull down control
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *control pointer to a pull down control buffer
 * @return     status code
 *             - 0 success
 *             - 1 get pull down control failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_pull_down_control(sx1262_handle_t *handle, uint8_t *control)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                       /* check handle */
    {
        return 2;                                                                                             /* return error */
    }
    if (handle->inited != 1)                                                                                  /* check handle initialization */
    {
        return 3;                                                                                             /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                        /* check busy */
    if (res != 0)                                                                                             /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                       /* chip is busy */
       
        return 4;                                                                                             /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_DIOX_PULL_DOWN_CONTROL, (uint8_t *)control, 1);       /* read register */
    if (res != 0)                                                                                             /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                               /* read register failed */
       
        return 1;                                                                                             /* return error */
    }
    
    return 0;                                                                                                 /* success return 0 */
}

/**
 * @brief     set fhss hopping enable
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] enable set params
 * @return    status code
 *            - 0 success
 *            - 1 set fhss hopping enable failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_hopping_enable(sx1262_handle_t *handle, uint8_t enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                  /* check busy */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                                 /* chip is busy */
       
        return 4;                                                                                       /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_HOPPING_ENABLE, (uint8_t *)&enable, 1);        /* write register */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                        /* write register failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

/**
 * @brief      get fhss hopping enable
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *enable pointer to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss hopping enable failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_hopping_enable(sx1262_handle_t *handle, uint8_t *enable)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_HOPPING_ENABLE, (uint8_t *)enable, 1);        /* read register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                       /* read register failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief     set fhss packet length
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] len set length
 * @return    status code
 *            - 0 success
 *            - 1 set fhss packet length failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_packet_length(sx1262_handle_t *handle, uint8_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                                             /* check handle */
    {
        return 2;                                                                                   /* return error */
    }
    if (handle->inited != 1)                                                                        /* check handle initialization */
    {
        return 3;                                                                                   /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                              /* check busy */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                             /* chip is busy */
       
        return 4;                                                                                   /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_PACKET_LENGTH, (uint8_t *)&len, 1);        /* write register */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                    /* write register failed */
       
        return 1;                                                                                   /* return error */
    }
    
    return 0;                                                                                       /* success return 0 */
}

/**
 * @brief      get fhss packet length
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *len pointer to a length buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss packet length failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_packet_length(sx1262_handle_t *handle, uint8_t *len)
{
    uint8_t res;
    
    if (handle == NULL)                                                                           /* check handle */
    {
        return 2;                                                                                 /* return error */
    }
    if (handle->inited != 1)                                                                      /* check handle initialization */
    {
        return 3;                                                                                 /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                            /* check busy */
    if (res != 0)                                                                                 /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                           /* chip is busy */
       
        return 4;                                                                                 /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_PACKET_LENGTH, (uint8_t *)len, 1);        /* read register */
    if (res != 0)                                                                                 /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                   /* read register failed */
       
        return 1;                                                                                 /* return error */
    }
    
    return 0;                                                                                     /* success return 0 */
}

/**
 * @brief     set fhss nb hopping blocks
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] n set blocks
 * @return    status code
 *            - 0 success
 *            - 1 set fhss nb hopping blocks failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_nb_hopping_blocks(sx1262_handle_t *handle, uint8_t n)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                                /* check busy */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                               /* chip is busy */
       
        return 4;                                                                                     /* return error */
    }
    
    res = a_sx1262_spi_write_register(handle, SX1262_REG_NB_HOPPING_BLOCKS, (uint8_t *)&n, 1);        /* write register */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                                      /* write register failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

/**
 * @brief      get fhss nb hopping blocks
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *n pointer to a blocks buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss nb hopping blocks failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_nb_hopping_blocks(sx1262_handle_t *handle, uint8_t *n)
{
    uint8_t res;
    
    if (handle == NULL)                                                                             /* check handle */
    {
        return 2;                                                                                   /* return error */
    }
    if (handle->inited != 1)                                                                        /* check handle initialization */
    {
        return 3;                                                                                   /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                              /* check busy */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                             /* chip is busy */
       
        return 4;                                                                                   /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_NB_HOPPING_BLOCKS, (uint8_t *)n, 1);        /* read register */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                                     /* read register failed */
       
        return 1;                                                                                   /* return error */
    }
    
    return 0;                                                                                       /* success return 0 */
}

/**
 * @brief     set fhss freq0 symbols
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] num set symbols number
 * @return    status code
 *            - 0 success
 *            - 1 set fhss symbols freq0 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_symbols_freq0(sx1262_handle_t *handle, uint16_t num)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                /* check handle */
    {
        return 2;                                                                      /* return error */
    }
    if (handle->inited != 1)                                                           /* check handle initialization */
    {
        return 3;                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                 /* check busy */
    if (res != 0)                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                /* chip is busy */
       
        return 4;                                                                      /* return error */
    }
    
    buf[0] = (num >> 8) & 0xFF;                                                        /* set msb */
    buf[1] = (num >> 0) & 0xFF;                                                        /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_NB_SYMBOLS_0, buf, 2);        /* write register */
    if (res != 0)                                                                      /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                       /* write register failed */
       
        return 1;                                                                      /* return error */
    }
    
    return 0;                                                                          /* success return 0 */
}

/**
 * @brief      get fhss freq0 symbols
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *num pointer to a symbols number buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss symbols freq0 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_symbols_freq0(sx1262_handle_t *handle, uint16_t *num)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                               /* check handle */
    {
        return 2;                                                                     /* return error */
    }
    if (handle->inited != 1)                                                          /* check handle initialization */
    {
        return 3;                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                /* check busy */
    if (res != 0)                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                               /* chip is busy */
       
        return 4;                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_NB_SYMBOLS_0, buf, 2);        /* read register */
    if (res != 0)                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                       /* read register failed */
       
        return 1;                                                                     /* return error */
    }
    *num = (uint16_t)(buf[0]) << 8 | buf[1];                                          /* get number */
    
    return 0;                                                                         /* success return 0 */
}

/**
 * @brief     set fhss freq0
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] freq set freq
 * @return    status code
 *            - 0 success
 *            - 1 set fhss freq0 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_freq0(sx1262_handle_t *handle, uint32_t freq)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                           /* check busy */
    if (res != 0)                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                          /* chip is busy */
       
        return 4;                                                                /* return error */
    }
    
    buf[0] = (freq >> 24) & 0xFF;                                                /* set part0 */
    buf[1] = (freq >> 16) & 0xFF;                                                /* set part1 */
    buf[2] = (freq >> 8) & 0xFF;                                                 /* set part2 */
    buf[3] = (freq >> 0) & 0xFF;                                                 /* set part3 */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_FREQ_0, buf, 4);        /* write register */
    if (res != 0)                                                                /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                 /* write register failed */
       
        return 1;                                                                /* return error */
    }
    
    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get fhss freq0
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *freq pointer to a freq buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss freq0 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_freq0(sx1262_handle_t *handle, uint32_t *freq)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                          /* check busy */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                         /* chip is busy */
       
        return 4;                                                               /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_FREQ_0, buf, 4);        /* read register */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                 /* read register failed */
       
        return 1;                                                               /* return error */
    }
    *freq = (uint32_t)(buf[0]) << 24 | (uint32_t)(buf[1]) << 16 |
            (uint32_t)(buf[2]) << 8 | (uint32_t)(buf[3]) << 0;                  /* get freq */
    
    return 0;                                                                   /* success return 0 */
}

/**
 * @brief     set fhss freq15 symbols
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] num set symbols number
 * @return    status code
 *            - 0 success
 *            - 1 set fhss symbols freq15 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_symbols_freq15(sx1262_handle_t *handle, uint16_t num)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                                /* check handle */
    {
        return 2;                                                                      /* return error */
    }
    if (handle->inited != 1)                                                           /* check handle initialization */
    {
        return 3;                                                                      /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                 /* check busy */
    if (res != 0)                                                                      /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                                /* chip is busy */
       
        return 4;                                                                      /* return error */
    }
    
    buf[0] = (num >> 8) & 0xFF;                                                        /* set msb */
    buf[1] = (num >> 0) & 0xFF;                                                        /* set lsb */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_NB_SYMBOLS_15, buf, 2);       /* write register */
    if (res != 0)                                                                      /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                       /* write register failed */
       
        return 1;                                                                      /* return error */
    }
    
    return 0;                                                                          /* success return 0 */
}

/**
 * @brief      get fhss freq15 symbols
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *num pointer to a number buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss symbols freq15 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_symbols_freq15(sx1262_handle_t *handle, uint16_t *num)
{
    uint8_t res;
    uint8_t buf[2];
    
    if (handle == NULL)                                                               /* check handle */
    {
        return 2;                                                                     /* return error */
    }
    if (handle->inited != 1)                                                          /* check handle initialization */
    {
        return 3;                                                                     /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                                /* check busy */
    if (res != 0)                                                                     /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                               /* chip is busy */
       
        return 4;                                                                     /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_NB_SYMBOLS_15, buf, 2);       /* read register */
    if (res != 0)                                                                     /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                       /* read register failed */
       
        return 1;                                                                     /* return error */
    }
    *num = (uint16_t)(buf[0]) << 8 | buf[1];                                          /* get number */
    
    return 0;                                                                         /* success return 0 */
}

/**
 * @brief     set fhss freq15
 * @param[in] *handle pointer to an sx1262 handle structure
 * @param[in] freq set freq
 * @return    status code
 *            - 0 success
 *            - 1 set fhss freq15 failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 chip is busy
 * @note      none
 */
uint8_t sx1262_set_fhss_freq15(sx1262_handle_t *handle, uint32_t freq)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                           /* check busy */
    if (res != 0)                                                                /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                          /* chip is busy */
       
        return 4;                                                                /* return error */
    }
    
    buf[0] = (freq >> 24) & 0xFF;                                                /* set part0 */
    buf[1] = (freq >> 16) & 0xFF;                                                /* set part1 */
    buf[2] = (freq >> 8) & 0xFF;                                                 /* set part2 */
    buf[3] = (freq >> 0) & 0xFF;                                                 /* set part3 */
    res = a_sx1262_spi_write_register(handle, SX1262_REG_FREQ_15, buf, 4);       /* write register */
    if (res != 0)                                                                /* check result */
    {
        handle->debug_print("sx1262: write register failed.\n");                 /* write register failed */
       
        return 1;                                                                /* return error */
    }
    
    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      get fhss freq15
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[out] *freq pointer to a freq buffer
 * @return     status code
 *             - 0 success
 *             - 1 get fhss freq15 failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 chip is busy
 * @note       none
 */
uint8_t sx1262_get_fhss_freq15(sx1262_handle_t *handle, uint32_t *freq)
{
    uint8_t res;
    uint8_t buf[4];
    
    if (handle == NULL)                                                         /* check handle */
    {
        return 2;                                                               /* return error */
    }
    if (handle->inited != 1)                                                    /* check handle initialization */
    {
        return 3;                                                               /* return error */
    }
    
    res = a_sx1262_check_busy(handle);                                          /* check busy */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("sx1262: chip is busy.\n");                         /* chip is busy */
       
        return 4;                                                               /* return error */
    }
    
    res = a_sx1262_spi_read_register(handle, SX1262_REG_FREQ_15, buf, 4);       /* read register */
    if (res != 0)                                                               /* check result */
    {
        handle->debug_print("sx1262: read register failed.\n");                 /* read register failed */
       
        return 1;                                                               /* return error */
    }
    *freq = (uint32_t)(buf[0]) << 24 | (uint32_t)(buf[1]) << 16 |
            (uint32_t)(buf[2]) << 8 | (uint32_t)(buf[3]) << 0;                  /* get freq */
    
    return 0;                                                                   /* success return 0 */
}

/**
 * @brief      write and read register
 * @param[in]  *handle pointer to an sx1262 handle structure
 * @param[in]  *in_buf pointer to an input buffer
 * @param[in]  in_len input length
 * @param[out] *out_buf pointer to an output buffer
 * @param[in]  out_len output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sx1262_write_read_reg(sx1262_handle_t *handle, uint8_t *in_buf, uint32_t in_len,
                              uint8_t *out_buf, uint32_t out_len)
{
    
    if (handle == NULL)                                                  /* check handle */
    {
        return 2;                                                        /* return error */
    }   
    if (handle->inited != 1)                                             /* check handle initialization */
    {
        return 3;                                                        /* return error */
    }
    
    if (handle->spi_write_read(in_buf, in_len, out_buf, out_len) != 0)   /* spi read */
    {
        return 1;                                                        /* return error */
    }
    else
    {
        return 0;                                                        /* success return 0 */
    }
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to an sx1262 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sx1262_info(sx1262_info_t *info)
{
    if (info == NULL)                                             /* check handle */
    {
        return 2;                                                 /* return error */
    }
    
    memset(info, 0, sizeof(sx1262_info_t));                       /* initialize sx1262 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                      /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);      /* copy manufacturer name */
    strncpy(info->interface, "SPI", 8);                           /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;              /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;              /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                           /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                      /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                      /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                        /* set driver version */
    
    return 0;                                                     /* success return 0 */
}
