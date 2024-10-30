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
 * @file      driver_sx1262_interface_template.c
 * @brief     driver sx1262 interface template source file
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
 
#include "driver_sx1262_interface.h"

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t sx1262_interface_spi_init(void)
{
    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t sx1262_interface_spi_deinit(void)
{
    return 0;
}

/**
 * @brief      interface spi bus write read
 * @param[in]  *in_buf points to a input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_buf points to a output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t sx1262_interface_spi_write_read(uint8_t *in_buf, uint32_t in_len,
                                        uint8_t *out_buf, uint32_t out_len)
{
    return 0;
}

/**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t sx1262_interface_reset_gpio_init(void)
{
    return 0;
}

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sx1262_interface_reset_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief     interface reset gpio write
 * @param[in] data is the written data
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t sx1262_interface_reset_gpio_write(uint8_t data)
{
    return 0;
}

/**
 * @brief  interface busy gpio init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t sx1262_interface_busy_gpio_init(void)
{
    return 0;
}

/**
 * @brief  interface busy gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sx1262_interface_busy_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief      interface busy gpio read
 * @param[out] *value points to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sx1262_interface_busy_gpio_read(uint8_t *value)
{
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void sx1262_interface_delay_ms(uint32_t ms)
{

}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void sx1262_interface_debug_print(const char *const fmt, ...)
{

}

/**
 * @brief     interface receive callback
 * @param[in] type is the receive callback type
 * @param[in] *buf points to a buffer address
 * @param[in] len is the buffer length
 * @note      none
 */
void sx1262_interface_receive_callback(uint16_t type, uint8_t *buf, uint16_t len)
{
    switch (type)
    {
        case SX1262_IRQ_TX_DONE :
        {
            sx1262_interface_debug_print("sx1262: irq tx done.\n");
            
            break;
        }
        case SX1262_IRQ_RX_DONE :
        {
            sx1262_interface_debug_print("sx1262: irq rx done.\n");
            
            break;
        }
        case SX1262_IRQ_PREAMBLE_DETECTED :
        {
            sx1262_interface_debug_print("sx1262: irq preamble detected.\n");
            
            break;
        }
        case SX1262_IRQ_SYNC_WORD_VALID :
        {
            sx1262_interface_debug_print("sx1262: irq valid sync word detected.\n");
            
            break;
        }
        case SX1262_IRQ_HEADER_VALID :
        {
            sx1262_interface_debug_print("sx1262: irq valid header.\n");
            
            break;
        }
        case SX1262_IRQ_HEADER_ERR :
        {
            sx1262_interface_debug_print("sx1262: irq header error.\n");
            
            break;
        }
        case SX1262_IRQ_CRC_ERR :
        {
            sx1262_interface_debug_print("sx1262: irq crc error.\n");
            
            break;
        }
        case SX1262_IRQ_CAD_DONE :
        {
            sx1262_interface_debug_print("sx1262: irq cad done.\n");
            
            break;
        }
        case SX1262_IRQ_CAD_DETECTED :
        {
            sx1262_interface_debug_print("sx1262: irq cad detected.\n");
            
            break;
        }
        case SX1262_IRQ_TIMEOUT :
        {
            sx1262_interface_debug_print("sx1262: irq timeout.\n");
            
            break;
        }
        default :
        {
            sx1262_interface_debug_print("sx1262: unknown code.\n");
            
            break;
        }
    }
}
