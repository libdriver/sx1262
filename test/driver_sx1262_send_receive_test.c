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
 * @file      driver_sx1262_send_receive_test.c
 * @brief     driver sx1262 send receive test source file
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

#include "driver_sx1262_send_receive_test.h"

static sx1262_handle_t gs_handle;        /**< sx1262 handle */
static uint8_t gs_rx_done;               /**< rx done */
static uint8_t gs_send_buffer[256];      /**< inner send buffer*/

/**
 * @brief     interface receive callback
 * @param[in] type is the receive callback type
 * @param[in] *buf points to a buffer address
 * @param[in] len is the buffer length
 * @note      none
 */
static void a_callback(uint16_t type, uint8_t *buf, uint16_t len)
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
            uint16_t i;
            sx1262_bool_t enable;
            uint8_t rssi_pkt_raw;
            uint8_t snr_pkt_raw;
            uint8_t signal_rssi_pkt_raw;
            float rssi_pkt;
            float snr_pkt;
            float signal_rssi_pkt;
            
            sx1262_interface_debug_print("sx1262: irq rx done.\n");
            
            /* get the status */
            if (sx1262_get_lora_packet_status(&gs_handle, (uint8_t *)&rssi_pkt_raw, (uint8_t *)&snr_pkt_raw,
                                             (uint8_t *)&signal_rssi_pkt_raw, (float *)&rssi_pkt,
                                             (float *)&snr_pkt, (float *)&signal_rssi_pkt) != 0)
            {
                return;
            }
            sx1262_interface_debug_print("sx1262: rssi is %0.1f.\n", rssi_pkt);
            sx1262_interface_debug_print("sx1262: snr is %0.2f.\n", snr_pkt);
            sx1262_interface_debug_print("sx1262: signal rssi is %0.1f.\n", signal_rssi_pkt);
            
            /* check the error */
            if (sx1262_check_packet_error(&gs_handle, &enable) != 0)
            {
                return;
            }
            if ((enable == SX1262_BOOL_FALSE) && len)
            {
                for (i = 0; i < len; i++)
                {
                    sx1262_interface_debug_print("0x%02X ", buf[i]);
                }
                sx1262_interface_debug_print("\n");
                gs_rx_done = 1;
            }
            
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
            break;
        }
    }
}

/**
 * @brief  sx1262 interrupt test irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t sx1262_interrupt_test_irq_handler(void)
{
    if (sx1262_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  send test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t sx1262_send_test(void)
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    uint8_t i;
    
    /* link interface function */
    DRIVER_SX1262_LINK_INIT(&gs_handle, sx1262_handle_t);
    DRIVER_SX1262_LINK_SPI_INIT(&gs_handle, sx1262_interface_spi_init);
    DRIVER_SX1262_LINK_SPI_DEINIT(&gs_handle, sx1262_interface_spi_deinit);
    DRIVER_SX1262_LINK_SPI_WRITE_READ(&gs_handle, sx1262_interface_spi_write_read);
    DRIVER_SX1262_LINK_RESET_GPIO_INIT(&gs_handle, sx1262_interface_reset_gpio_init);
    DRIVER_SX1262_LINK_RESET_GPIO_DEINIT(&gs_handle, sx1262_interface_reset_gpio_deinit);
    DRIVER_SX1262_LINK_RESET_GPIO_WRITE(&gs_handle, sx1262_interface_reset_gpio_write);
    DRIVER_SX1262_LINK_BUSY_GPIO_INIT(&gs_handle, sx1262_interface_busy_gpio_init);
    DRIVER_SX1262_LINK_BUSY_GPIO_DEINIT(&gs_handle, sx1262_interface_busy_gpio_deinit);
    DRIVER_SX1262_LINK_BUSY_GPIO_READ(&gs_handle, sx1262_interface_busy_gpio_read);
    DRIVER_SX1262_LINK_DELAY_MS(&gs_handle, sx1262_interface_delay_ms);
    DRIVER_SX1262_LINK_DEBUG_PRINT(&gs_handle, sx1262_interface_debug_print);
    DRIVER_SX1262_LINK_RECEIVE_CALLBACK(&gs_handle, sx1262_interface_receive_callback);
    
    /* start send test */
    sx1262_interface_debug_print("sx1262: start send test.\n");
    
    /* init the sx1262 */
    res = sx1262_init(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: init failed.\n");
       
        return 1;
    }
    
    /* enter standby */
    res = sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set standby failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* disable stop timer on preamble */
    res = sx1262_set_stop_timer_on_preamble(&gs_handle, SX1262_BOOL_FALSE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: stop timer on preamble failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set dc dc ldo */
    res = sx1262_set_regulator_mode(&gs_handle, SX1262_REGULATOR_MODE_DC_DC_LDO);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set regulator mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set +17dBm power */
    res = sx1262_set_pa_config(&gs_handle, 0x02, 0x03);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pa config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enter to stdby rc mode */
    res = sx1262_set_rx_tx_fallback_mode(&gs_handle, SX1262_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx tx fallback mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set dio irq */
    res = sx1262_set_dio_irq_params(&gs_handle, 0x03FFU, 0x03FFU, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio irq params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear irq status */
    res = sx1262_clear_irq_status(&gs_handle, 0x03FFU);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear irq status failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set lora mode */
    res = sx1262_set_packet_type(&gs_handle, SX1262_PACKET_TYPE_LORA);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* +17dBm */
    res = sx1262_set_tx_params(&gs_handle, 17, SX1262_RAMP_TIME_10US);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* sf9, 125khz, cr4/5, disable low data rate optimize */
    res = sx1262_set_lora_modulation_params(&gs_handle, SX1262_LORA_SF_9, SX1262_LORA_BANDWIDTH_125_KHZ, 
                                            SX1262_LORA_CR_4_5, SX1262_BOOL_FALSE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora modulation params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* convert the frequency */
    res = sx1262_frequency_convert_to_register(&gs_handle, 480100000U, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the frequency */
    res = sx1262_set_rf_frequency(&gs_handle, reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rf frequency failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set base address */
    res = sx1262_set_buffer_base_address(&gs_handle, 0x00, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set buffer base address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* 1 lora symb num */
    res = sx1262_set_lora_symb_num_timeout(&gs_handle, 0);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora symb num timeout failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* reset stats */
    res = sx1262_reset_stats(&gs_handle, 0x0000, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: reset stats failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear device errors */
    res = sx1262_clear_device_errors(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear device errors failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the lora sync word */
    res = sx1262_set_lora_sync_word(&gs_handle, 0x1424U);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* get tx modulation */
    res = sx1262_get_tx_modulation(&gs_handle, (uint8_t *)&modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    modulation |= 0x04;
    
    /* set the tx modulation */
    res = sx1262_set_tx_modulation(&gs_handle, modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the rx gain */
    res = sx1262_set_rx_gain(&gs_handle, 0x94);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the ocp */
    res = sx1262_set_ocp(&gs_handle, 0x38);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set ocp failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* get the tx clamp config */
    res = sx1262_get_tx_clamp_config(&gs_handle, (uint8_t *)&config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    config |= 0x1E;
    
    /* set the tx clamp config */
    res = sx1262_set_tx_clamp_config(&gs_handle, config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* generate the buffer */
    for (i = 0; i < 192; i++)
    {
        gs_send_buffer[i] = i;
    }
    
    /* send the data */
    res = sx1262_lora_transmit(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ,
                               50, SX1262_LORA_HEADER_EXPLICIT,
                               SX1262_LORA_CRC_TYPE_ON, SX1262_BOOL_FALSE,
                              (uint8_t *)gs_send_buffer, 192, 0);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: lora send failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* finish send test */
    sx1262_interface_debug_print("sx1262: finish send test.\n");
    
    /* deinit */
    (void)sx1262_deinit(&gs_handle);
    
    return 0;
}

/**
 * @brief     receive test
 * @param[in] s is the timeout time
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t sx1262_receive_test(uint32_t s)
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    uint8_t setup;
    
    /* link interface function */
    DRIVER_SX1262_LINK_INIT(&gs_handle, sx1262_handle_t);
    DRIVER_SX1262_LINK_SPI_INIT(&gs_handle, sx1262_interface_spi_init);
    DRIVER_SX1262_LINK_SPI_DEINIT(&gs_handle, sx1262_interface_spi_deinit);
    DRIVER_SX1262_LINK_SPI_WRITE_READ(&gs_handle, sx1262_interface_spi_write_read);
    DRIVER_SX1262_LINK_RESET_GPIO_INIT(&gs_handle, sx1262_interface_reset_gpio_init);
    DRIVER_SX1262_LINK_RESET_GPIO_DEINIT(&gs_handle, sx1262_interface_reset_gpio_deinit);
    DRIVER_SX1262_LINK_RESET_GPIO_WRITE(&gs_handle, sx1262_interface_reset_gpio_write);
    DRIVER_SX1262_LINK_BUSY_GPIO_INIT(&gs_handle, sx1262_interface_busy_gpio_init);
    DRIVER_SX1262_LINK_BUSY_GPIO_DEINIT(&gs_handle, sx1262_interface_busy_gpio_deinit);
    DRIVER_SX1262_LINK_BUSY_GPIO_READ(&gs_handle, sx1262_interface_busy_gpio_read);
    DRIVER_SX1262_LINK_DELAY_MS(&gs_handle, sx1262_interface_delay_ms);
    DRIVER_SX1262_LINK_DEBUG_PRINT(&gs_handle, sx1262_interface_debug_print);
    DRIVER_SX1262_LINK_RECEIVE_CALLBACK(&gs_handle, a_callback);
    
    /* start receive test */
    sx1262_interface_debug_print("sx1262: start receive test.\n");
    
    /* init the sx1262 */
    res = sx1262_init(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: init failed.\n");
       
        return 1;
    }
    
    /* enter standby */
    res = sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set standby failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* disable stop timer on preamble */
    res = sx1262_set_stop_timer_on_preamble(&gs_handle, SX1262_BOOL_FALSE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: stop timer on preamble failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set dc dc ldo */
    res = sx1262_set_regulator_mode(&gs_handle, SX1262_REGULATOR_MODE_DC_DC_LDO);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set regulator mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set +17dBm power */
    res = sx1262_set_pa_config(&gs_handle, 0x02, 0x03);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pa config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enter to stdby rc mode */
    res = sx1262_set_rx_tx_fallback_mode(&gs_handle, SX1262_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx tx fallback mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set dio irq */
    res = sx1262_set_dio_irq_params(&gs_handle, 0x03FFU, 0x03FFU, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio irq params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear irq status */
    res = sx1262_clear_irq_status(&gs_handle, 0x03FFU);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear irq status failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set lora mode */
    res = sx1262_set_packet_type(&gs_handle, SX1262_PACKET_TYPE_LORA);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* +17dBm */
    res = sx1262_set_tx_params(&gs_handle, 17, SX1262_RAMP_TIME_10US);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* sf9, 125khz, cr4/5, disable low data rate optimize */
    res = sx1262_set_lora_modulation_params(&gs_handle, SX1262_LORA_SF_9, SX1262_LORA_BANDWIDTH_125_KHZ, 
                                            SX1262_LORA_CR_4_5, SX1262_BOOL_FALSE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora modulation params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* convert the frequency */
    res = sx1262_frequency_convert_to_register(&gs_handle, 480100000U, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the frequency */
    res = sx1262_set_rf_frequency(&gs_handle, reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rf frequency failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set base address */
    res = sx1262_set_buffer_base_address(&gs_handle, 0x00, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set buffer base address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set lora symb num */
    res = sx1262_set_lora_symb_num_timeout(&gs_handle, 0);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora symb num timeout failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* reset stats */
    res = sx1262_reset_stats(&gs_handle, 0x0000, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: reset stats failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear device errors */
    res = sx1262_clear_device_errors(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear device errors failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the lora sync word */
    res = sx1262_set_lora_sync_word(&gs_handle, 0x1424U);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* get tx modulation */
    res = sx1262_get_tx_modulation(&gs_handle, (uint8_t *)&modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    modulation |= 0x04;
    
    /* set the tx modulation */
    res = sx1262_set_tx_modulation(&gs_handle, modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the rx gain */
    res = sx1262_set_rx_gain(&gs_handle, 0x94);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the ocp */
    res = sx1262_set_ocp(&gs_handle, 0x38);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set ocp failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* get the tx clamp config */
    res = sx1262_get_tx_clamp_config(&gs_handle, (uint8_t *)&config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    config |= 0x1E;
    
    /* set the tx clamp config */
    res = sx1262_set_tx_clamp_config(&gs_handle, config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set lora packet params */
    res = sx1262_set_lora_packet_params(&gs_handle, 50,
                                        SX1262_LORA_HEADER_EXPLICIT, 255,
                                        SX1262_LORA_CRC_TYPE_ON, SX1262_BOOL_FALSE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora packet params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* get iq polarity */
    res = sx1262_get_iq_polarity(&gs_handle, (uint8_t *)&setup);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get iq polarity failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    setup |= 1 << 2;
    
    /* set the iq polarity */
    res = sx1262_set_iq_polarity(&gs_handle, setup);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set iq polarity failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* start receive */
    res = sx1262_continuous_receive(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: lora continuous receive failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* start receiving */
    sx1262_interface_debug_print("sx1262: start receiving...\n");
    gs_rx_done = 0;
    
    while ((s != 0) && (gs_rx_done == 0))
    {
        s--;
        sx1262_interface_delay_ms(1000);
    }
    if (gs_rx_done == 0)
    {
        /* receive timeout */
        sx1262_interface_debug_print("sx1262: receive timeout.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    else
    {
        /* finish receive test */
        sx1262_interface_debug_print("sx1262: finish receive test.\n");
        (void)sx1262_deinit(&gs_handle);
    }
    
    return 0;
}
