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
 * @file      driver_sx1262_lora.c
 * @brief     driver sx1262 lora source file
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

#include "driver_sx1262_lora.h"

static sx1262_handle_t gs_handle;        /**< sx1262 handle */

/**
 * @brief  sx1262 lora irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t sx1262_lora_irq_handler(void)
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
 * @brief     lora example init
 * @param[in] *callback points to a callback address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t sx1262_lora_init(void (*callback)(uint16_t type, uint8_t *buf, uint16_t len))
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    
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
    DRIVER_SX1262_LINK_RECEIVE_CALLBACK(&gs_handle, callback);
    
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
    
    /* set stop timer on preamble */
    res = sx1262_set_stop_timer_on_preamble(&gs_handle, SX1262_LORA_DEFAULT_STOP_TIMER_ON_PREAMBLE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: stop timer on preamble failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set regulator mode */
    res = sx1262_set_regulator_mode(&gs_handle, SX1262_LORA_DEFAULT_REGULATOR_MODE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set regulator mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set pa config */
    res = sx1262_set_pa_config(&gs_handle, SX1262_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE, SX1262_LORA_DEFAULT_PA_CONFIG_HP_MAX);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pa config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* enter to stdby xosc mode */
    res = sx1262_set_rx_tx_fallback_mode(&gs_handle, SX1262_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx tx fallback mode failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set dio irq */
    res = sx1262_set_dio_irq_params(&gs_handle, 0x03FF, 0x03FF, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio irq params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* clear irq status */
    res = sx1262_clear_irq_status(&gs_handle, 0x03FF);
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
    
    /* set tx params */
    res = sx1262_set_tx_params(&gs_handle, SX1262_LORA_DEFAULT_TX_DBM, SX1262_LORA_DEFAULT_RAMP_TIME);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set lora modulation params */
    res = sx1262_set_lora_modulation_params(&gs_handle, SX1262_LORA_DEFAULT_SF, SX1262_LORA_DEFAULT_BANDWIDTH, 
                                            SX1262_LORA_DEFAULT_CR, SX1262_LORA_DEFAULT_LOW_DATA_RATE_OPTIMIZE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora modulation params failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* convert the frequency */
    res = sx1262_frequency_convert_to_register(&gs_handle, SX1262_LORA_DEFAULT_RF_FREQUENCY, (uint32_t *)&reg);
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
    res = sx1262_set_lora_symb_num_timeout(&gs_handle, SX1262_LORA_DEFAULT_SYMB_NUM_TIMEOUT);
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
    res = sx1262_set_lora_sync_word(&gs_handle, SX1262_LORA_DEFAULT_SYNC_WORD);
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
    res = sx1262_set_rx_gain(&gs_handle, SX1262_LORA_DEFAULT_RX_GAIN);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set the ocp */
    res = sx1262_set_ocp(&gs_handle, SX1262_LORA_DEFAULT_OCP);
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
    
    return 0;
}

/**
 * @brief  lora example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sx1262_lora_deinit(void)
{
    if (sx1262_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  lora example enter to the sleep mode
 * @return status code
 *         - 0 success
 *         - 1 sleep failed
 * @note   none
 */
uint8_t sx1262_lora_sleep(void)
{
    if (sx1262_set_sleep(&gs_handle, SX1262_LORA_DEFAULT_START_MODE, SX1262_LORA_DEFAULT_RTC_WAKE_UP) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  lora example wake up the chip
 * @return status code
 *         - 0 success
 *         - 1 wake up failed
 * @note   none
 */
uint8_t sx1262_lora_wake_up(void)
{
    uint8_t status;
    
    if (sx1262_get_status(&gs_handle, (uint8_t *)&status) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  lora example enter to the continuous receive mode
 * @return status code
 *         - 0 success
 *         - 1 enter failed
 * @note   none
 */
uint8_t sx1262_lora_set_continuous_receive_mode(void)
{
    uint8_t setup;
    
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&gs_handle, SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* set lora packet params */
    if (sx1262_set_lora_packet_params(&gs_handle, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
                                      SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* get iq polarity */
    if (sx1262_get_iq_polarity(&gs_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if SX1262_LORA_DEFAULT_INVERT_IQ == SX1262_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* set the iq polarity */
    if (sx1262_set_iq_polarity(&gs_handle, setup) != 0)
    {
        return 1;
    }
    
    /* start receive */
    if (sx1262_continuous_receive(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  lora example enter to the shot receive mode
 * @return status code
 *         - 0 success
 *         - 1 enter failed
 * @note   none
 */
uint8_t sx1262_lora_set_shot_receive_mode(double us)
{
    uint8_t setup;
    
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&gs_handle, SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    /* set lora packet params */
    if (sx1262_set_lora_packet_params(&gs_handle, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
                                      SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }
    
    /* get iq polarity */
    if (sx1262_get_iq_polarity(&gs_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }
    
#if SX1262_LORA_DEFAULT_INVERT_IQ == SX1262_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif
    
    /* set the iq polarity */
    if (sx1262_set_iq_polarity(&gs_handle, setup) != 0)
    {
        return 1;
    }
    
    /* start receive */
    if (sx1262_single_receive(&gs_handle, us) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  lora example enter to the send mode
 * @return status code
 *         - 0 success
 *         - 1 enter failed
 * @note   none
 */
uint8_t sx1262_lora_set_send_mode(void)
{
    /* set dio irq */
    if (sx1262_set_dio_irq_params(&gs_handle, SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }
    
    /* clear irq status */
    if (sx1262_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     lora example send lora data
 * @param[in] *buf points to a data buffer
 * @param[in] len is the data length
 * @return    status code
 *            - 0 success
 *            - 1 send failed
 * @note      none
 */
uint8_t sx1262_lora_send(uint8_t *buf, uint16_t len)
{
    /* send the data */
    if (sx1262_lora_transmit(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ,
                             SX1262_LORA_DEFAULT_PREAMBLE_LENGTH, SX1262_LORA_DEFAULT_HEADER,
                             SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ,
                            (uint8_t *)buf, len, 0) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example run the cad
 * @param[out] *enable points to a enable buffer
 * @return     status code
 *             - 0 success
 *             - 1 run failed
 * @note       none
 */
uint8_t sx1262_lora_run_cad(sx1262_bool_t *enable)
{
    /* set cad params */
    if (sx1262_set_cad_params(&gs_handle, SX1262_LORA_DEFAULT_CAD_SYMBOL_NUM,
                              SX1262_LORA_DEFAULT_CAD_DET_PEAK, SX1262_LORA_DEFAULT_CAD_DET_MIN,
                              SX1262_LORA_CAD_EXIT_MODE_ONLY, 0) != 0)
    {
        return 1;
    }
    
    /* run the cad */
    if (sx1262_lora_cad(&gs_handle, enable) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example get the status
 * @param[out] *rssi points to a rssi buffer
 * @param[out] *snr points to a snr buffer
 * @return     status code
 *             - 0 success
 *             - 1 get status failed
 * @note       none
 */
uint8_t sx1262_lora_get_status(float *rssi, float *snr)
{
    uint8_t rssi_pkt_raw;
    uint8_t snr_pkt_raw;
    uint8_t signal_rssi_pkt_raw;
    float signal_rssi_pkt;
    
    /* get the status */
    if (sx1262_get_lora_packet_status(&gs_handle, (uint8_t *)&rssi_pkt_raw, (uint8_t *)&snr_pkt_raw,
                                     (uint8_t *)&signal_rssi_pkt_raw, (float *)rssi, (float *)snr, (float *)&signal_rssi_pkt) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      lora example check packet error
 * @param[out] *enable points to a bool value buffer
 * @return     status code
 *             - 0 success
 *             - 1 check packet error failed
 * @note       none
 */
uint8_t sx1262_lora_check_packet_error(sx1262_bool_t *enable)
{
    /* check the error */
    if (sx1262_check_packet_error(&gs_handle, enable) != 0)
    {
        return 1;
    }

    return 0;
}
