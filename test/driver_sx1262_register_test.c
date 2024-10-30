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
 * @file      driver_sx1262_register_test.c
 * @brief     driver sx1262 register test source file
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

#include "driver_sx1262_register_test.h"
#include <stdlib.h>

static sx1262_handle_t gs_handle;        /**< sx1262 handle */

/**
 * @brief  register test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t sx1262_register_test(void)
{
    uint8_t res;
    uint8_t v, v_check;
    uint8_t addr, addr_check;
    uint8_t i;
    uint16_t value, value_check;
    uint8_t sync_word[8];
    uint8_t sync_word_check[8];
    uint8_t setup, setup_check;
    uint32_t rand_number;
    uint8_t modulation, modulation_check;
    uint8_t gain, gain_check;
    uint8_t config, config_check;
    uint8_t ocp, ocp_check;
    uint8_t control, control_check;
    uint8_t trim, trim_check;
    uint8_t mask, mask_check;
    uint8_t enable, enable_check;
    double us, us_check;
    uint32_t freq, freq_check;
    uint32_t br, br_check;
    uint32_t reg;
    sx1262_packet_type_t type;
    sx1262_info_t info;
    
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
    
    /* get information */
    res = sx1262_info(&info);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print chip info */
        sx1262_interface_debug_print("sx1262: chip is %s.\n", info.chip_name);
        sx1262_interface_debug_print("sx1262: manufacturer is %s.\n", info.manufacturer_name);
        sx1262_interface_debug_print("sx1262: interface is %s.\n", info.interface);
        sx1262_interface_debug_print("sx1262: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        sx1262_interface_debug_print("sx1262: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        sx1262_interface_debug_print("sx1262: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        sx1262_interface_debug_print("sx1262: max current is %0.2fmA.\n", info.max_current_ma);
        sx1262_interface_debug_print("sx1262: max temperature is %0.1fC.\n", info.temperature_max);
        sx1262_interface_debug_print("sx1262: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* start register test */
    sx1262_interface_debug_print("sx1262: start register test.\n");
    
    /* init the sx1262 */
    res = sx1262_init(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: init failed.\n");
       
        return 1;
    }
    
    /* set standby mode */
    res = sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set standby failed.\n");
        (void)sx1262_deinit(&gs_handle);
       
        return 1;
    }
    
    /* sx1262_write_register/sx1262_read_register test */
    sx1262_interface_debug_print("sx1262: sx1262_write_register/sx1262_read_register test.\n");
    
    control = rand() % 256;
    res = sx1262_write_register(&gs_handle, 0x06BC, (uint8_t *)&control, 1);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: write register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set register 0x%02X.\n", control);
    res = sx1262_read_register(&gs_handle, 0x06BC, (uint8_t *)&control_check, 1);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: read register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check register %s.\n", control_check == control ? "ok" : "error");
    
    /* sx1262_write_buffer/sx1262_read_buffer test */
    sx1262_interface_debug_print("sx1262: sx1262_write_buffer/sx1262_read_buffer test.\n");
    
    control = rand() % 256;
    res = sx1262_write_buffer(&gs_handle , 0, (uint8_t *)&control, 1);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: write buffer failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set buffer 0x%02X.\n", control);
    res = sx1262_read_buffer(&gs_handle, 0, (uint8_t *)&control_check, 1);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: read buffer failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check buffer %s.\n", control_check == control ? "ok" : "error");
    
    /* sx1262_timeout_convert_to_register/sx1262_timeout_convert_to_data test */
    sx1262_interface_debug_print("sx1262: sx1262_timeout_convert_to_register/sx1262_timeout_convert_to_data test.\n");
    
    us = (rand() % 10000) / 10.0;
    res = sx1262_timeout_convert_to_register(&gs_handle, us, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: timeout convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set timeout %f us.\n", us);
    res = sx1262_timeout_convert_to_data(&gs_handle, reg, (double *)&us_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: timeout convert to data failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check timeout %f us.\n", us_check);
    
    /* sx1262_frequency_convert_to_register/sx1262_frequency_convert_to_data test */
    sx1262_interface_debug_print("sx1262: sx1262_frequency_convert_to_register/sx1262_frequency_convert_to_data test.\n");
    
    freq = 47000000;
    res = sx1262_frequency_convert_to_register(&gs_handle, freq, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: frequency convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set frequency %d Hz.\n", freq);
    res = sx1262_frequency_convert_to_data(&gs_handle, reg, (uint32_t *)&freq_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: frequency convert to data failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check frequency %d Hz.\n", freq_check);
    
    /* sx1262_gfsk_bit_rate_convert_to_register/sx1262_gfsk_bit_rate_convert_to_data test */
    sx1262_interface_debug_print("sx1262: sx1262_gfsk_bit_rate_convert_to_register/sx1262_gfsk_bit_rate_convert_to_data test.\n");
    
    br = 470000;
    res = sx1262_gfsk_bit_rate_convert_to_register(&gs_handle, br, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: gfsk bit rate convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set bit rate %d Hz.\n", br);
    res = sx1262_gfsk_bit_rate_convert_to_data(&gs_handle, reg, (uint32_t *)&br_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: gfsk bit rate convert to data failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check bit rate %d Hz.\n", br_check);
    
    /* sx1262_gfsk_frequency_deviation_convert_to_register/sx1262_gfsk_frequency_deviation_convert_to_data test */
    sx1262_interface_debug_print("sx1262: sx1262_gfsk_frequency_deviation_convert_to_register/sx1262_gfsk_frequency_deviation_convert_to_data test.\n");
    
    freq = 10000;
    res = sx1262_gfsk_frequency_deviation_convert_to_register(&gs_handle, freq, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: gfsk frequency deviation convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: gfsk frequency deviation %d Hz.\n", freq);
    res = sx1262_gfsk_frequency_deviation_convert_to_data(&gs_handle, reg, (uint32_t *)&freq_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: gfsk frequency deviation convert to data failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check gfsk frequency deviation %d Hz.\n", freq_check);
    
    /* sx1262_set_packet_type/sx1262_get_packet_type test */
    sx1262_interface_debug_print("sx1262: sx1262_set_packet_type/sx1262_get_packet_type test.\n");
    
    /* set gfsk type */
    res = sx1262_set_packet_type(&gs_handle, SX1262_PACKET_TYPE_GFSK);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set gfsk type.\n");
    res = sx1262_get_packet_type(&gs_handle, &type);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check packet type %s.\n", type == SX1262_PACKET_TYPE_GFSK ? "ok" : "error");
    
    /* set lora type */
    res = sx1262_set_packet_type(&gs_handle, SX1262_PACKET_TYPE_LORA);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set lora type.\n");
    res = sx1262_get_packet_type(&gs_handle, &type);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check packet type %s.\n", type == SX1262_PACKET_TYPE_LORA ? "ok" : "error");
    
    /* sx1262_set_fsk_whitening_initial_value/sx1262_get_fsk_whitening_initial_value test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_whitening_initial_value/sx1262_get_fsk_whitening_initial_value test.\n");
    
    value = 0x0100;
    res = sx1262_set_fsk_whitening_initial_value(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk whitening initial value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk whitening initial value 0x%04X.\n", value);
    res = sx1262_get_fsk_whitening_initial_value(&gs_handle, (uint16_t *)&value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk whitening initial value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk whitening initial value %s.\n", value_check == value ? "ok" : "error");
    
    /* sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test.\n");
    
    value = rand() % 65536;
    res = sx1262_set_fsk_crc_initical_value(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk crc initical value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk crc initical value 0x%04X.\n", value);
    res = sx1262_get_fsk_crc_initical_value(&gs_handle, (uint16_t *)&value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk crc initical value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk crc initical value %s.\n", value_check == value ? "ok" : "error");
    
    /* sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test.\n");
    
    value = rand() % 65536;
    res = sx1262_set_fsk_crc_polynomial_value(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk crc polynomial value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk crc polynomial value 0x%04X.\n", value);
    res = sx1262_get_fsk_crc_polynomial_value(&gs_handle, (uint16_t *)&value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk crc polynomial value failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk crc polynomial value %s.\n", value_check == value ? "ok" : "error");
    
    /* sx1262_set_fsk_sync_word/sx1262_get_fsk_sync_word test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_sync_word/sx1262_get_fsk_sync_word test.\n");
    
    for (i = 0; i < 8; i++)
    {
        sync_word[i] = rand() %256;
    }
    res = sx1262_set_fsk_sync_word(&gs_handle, (uint8_t *)sync_word);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk sync word 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X.\n",
                                 sync_word[0], sync_word[1], sync_word[2], sync_word[3],
                                 sync_word[4], sync_word[5], sync_word[6], sync_word[7]);
    res = sx1262_get_fsk_sync_word(&gs_handle, (uint8_t *)sync_word_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk sync word %s.\n", memcmp((uint8_t *)sync_word, (uint8_t *)sync_word_check, 8) == 0 ? "ok" : "error");
    
    /* sx1262_set_fsk_node_address/sx1262_get_fsk_node_address test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_node_address/sx1262_get_fsk_node_address test.\n");
    
    addr = rand() %256;
    res = sx1262_set_fsk_node_address(&gs_handle, addr);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk node address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk node address 0x%02X.\n", addr);
    res = sx1262_get_fsk_node_address(&gs_handle, (uint8_t *)&addr_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk node address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk node address %s.\n", addr == addr_check ? "ok" : "error");
    
    /* sx1262_set_fsk_broadcast_address/sx1262_get_fsk_broadcast_address test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fsk_broadcast_address/sx1262_get_fsk_broadcast_address test.\n");
    
    addr = rand() %256;
    res = sx1262_set_fsk_broadcast_address(&gs_handle, addr);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fsk broadcast address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fsk broadcast address 0x%02X.\n", addr);
    res = sx1262_get_fsk_broadcast_address(&gs_handle, (uint8_t *)&addr_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fsk broadcast address failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fsk broadcast address %s.\n", addr == addr_check ? "ok" : "error");
    
    /* sx1262_set_iq_polarity/sx1262_get_iq_polarity test */
    sx1262_interface_debug_print("sx1262: sx1262_set_iq_polarity/sx1262_get_iq_polarity test.\n");
    
    setup = rand() %256;
    res = sx1262_set_iq_polarity(&gs_handle, setup);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set iq polarity failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set iq polarity 0x%02X.\n", setup);
    res = sx1262_get_iq_polarity(&gs_handle, (uint8_t *)&setup_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get iq polarity failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check iq polarity %s.\n", setup == setup_check ? "ok" : "error");
    
    /* sx1262_set_lora_sync_word/sx1262_get_lora_sync_word test */
    sx1262_interface_debug_print("sx1262: sx1262_set_lora_sync_word/sx1262_get_lora_sync_word test.\n");
    
    value = rand() % 65536;
    res = sx1262_set_lora_sync_word(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set lora sync word 0x%04X.\n", value);
    res = sx1262_get_lora_sync_word(&gs_handle, (uint16_t *)&value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get lora sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check lora sync word %s.\n", value == value_check ? "ok" : "error");
    
    /* sx1262_get_random_number test */
    sx1262_interface_debug_print("sx1262: sx1262_get_random_number test.\n");
    
    res = sx1262_get_random_number(&gs_handle, (uint32_t *)&rand_number);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get random number failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: random number is 0x%08X.\n", rand_number);
    
    /* sx1262_set_tx_modulation/sx1262_get_tx_modulation test */
    sx1262_interface_debug_print("sx1262: sx1262_set_tx_modulation/sx1262_get_tx_modulation test.\n");
    
    res = sx1262_get_tx_modulation(&gs_handle, (uint8_t *)&modulation_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    modulation = 0;
    modulation |= 0x04;
    res = sx1262_set_tx_modulation(&gs_handle, modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set tx modulation 0x%02X.\n", modulation);
    res = sx1262_get_tx_modulation(&gs_handle, (uint8_t *)&modulation_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check tx modulation %s.\n", modulation == modulation_check ? "ok" : "error");
    
    /* sx1262_set_rx_gain/sx1262_get_rx_gain test */
    sx1262_interface_debug_print("sx1262: sx1262_set_rx_gain/sx1262_get_rx_gain test.\n");
    
    gain = rand() % 256;
    res = sx1262_set_rx_gain(&gs_handle, gain);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set rx gain 0x%02X.\n", gain);
    res = sx1262_get_rx_gain(&gs_handle, (uint8_t *)&gain_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check rx gain %s.\n", gain == gain_check ? "ok" : "error");
    
    /* sx1262_set_tx_clamp_config/sx1262_get_tx_clamp_config test */
    sx1262_interface_debug_print("sx1262: sx1262_set_tx_clamp_config/sx1262_get_tx_clamp_config test.\n");
    
    config = rand() % 256;
    res = sx1262_set_tx_clamp_config(&gs_handle, config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set tx clamp config 0x%02X.\n", config);
    res = sx1262_get_tx_clamp_config(&gs_handle, (uint8_t *)&config_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check tx clamp config %s.\n", config == config_check ? "ok" : "error");
    
    /* sx1262_set_ocp/sx1262_get_ocp test */
    sx1262_interface_debug_print("sx1262: sx1262_set_ocp/sx1262_get_ocp test.\n");
    
    ocp = rand() % 0x20;
    res = sx1262_set_ocp(&gs_handle, ocp);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set ocp failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set ocp 0x%02X.\n", ocp);
    res = sx1262_get_ocp(&gs_handle, (uint8_t *)&ocp_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get ocp failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check ocp config %s.\n", ocp == ocp_check ? "ok" : "error");
    
    /* sx1262_set_rtc_control/sx1262_get_rtc_control test */
    sx1262_interface_debug_print("sx1262: sx1262_set_rtc_control/sx1262_get_rtc_control test.\n");
    
    /* disable */
    control = 0;
    res = sx1262_set_rtc_control(&gs_handle, control);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rtc control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: disable rtc control.\n");
    res = sx1262_get_rtc_control(&gs_handle, (uint8_t *)&control_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get rtc control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check rtc control %s.\n", control_check == control ? "ok" : "error");
    
    /* sx1262_set_xta_trim/sx1262_get_xta_trim test */
    sx1262_interface_debug_print("sx1262: sx1262_set_xta_trim/sx1262_get_xta_trim test.\n");
    
    trim = 0x05;
    res = sx1262_set_xta_trim(&gs_handle, trim);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set xta trim failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set xta trim 0x%02X.\n", trim);
    res = sx1262_get_xta_trim(&gs_handle, (uint8_t *)&trim_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get xta trim failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check xta trim %s.\n", trim_check == trim ? "ok" : "error");
    
    /* sx1262_set_xtb_trim/sx1262_get_xtb_trim test */
    sx1262_interface_debug_print("sx1262: sx1262_set_xtb_trim/sx1262_get_xtb_trim test.\n");
    
    trim = 0x05;
    res = sx1262_set_xtb_trim(&gs_handle, trim);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set xtb trim failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set xtb trim 0x%02X.\n", trim);
    res = sx1262_get_xtb_trim(&gs_handle, (uint8_t *)&trim_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get xtb trim failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check xtb trim %s.\n", trim_check == trim ? "ok" : "error");
    
    /* sx1262_set_dio3_output_control/sx1262_get_dio3_output_control test */
    sx1262_interface_debug_print("sx1262: sx1262_set_dio3_output_control/sx1262_get_dio3_output_control test.\n");
    
    control = 0x01;
    res = sx1262_set_dio3_output_control(&gs_handle, control);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio3 output control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set dio3 output control 0x%02X.\n", control);
    res = sx1262_get_dio3_output_control(&gs_handle, (uint8_t *)&control_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get dio3 output control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check dio3 output control %s.\n", control_check == control ? "ok" : "error");
    
    /* sx1262_set_event_mask/sx1262_get_event_mask test */
    sx1262_interface_debug_print("sx1262: sx1262_set_event_mask/sx1262_get_event_mask test.\n");
    
    mask = 0;
    res = sx1262_set_event_mask(&gs_handle, mask);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set event mask failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set event mask 0x%02X.\n", mask);
    res = sx1262_get_event_mask(&gs_handle, (uint8_t *)&mask_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get event mask failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check event mask %s.\n", mask_check == mask ? "ok" : "error");
    
    /* sx1262_set_dio_output_enable/sx1262_get_dio_output_enable test */
    sx1262_interface_debug_print("sx1262: sx1262_set_dio_output_enable/sx1262_get_dio_output_enable test.\n");
    
    enable = rand() % 0xF;
    res = sx1262_set_dio_output_enable(&gs_handle, enable);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio output enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set dio output enable 0x%02X.\n", enable);
    res = sx1262_get_dio_output_enable(&gs_handle, (uint8_t *)&enable_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get dio output enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check dio output enable %s.\n", enable == enable_check ? "ok" : "error");
    
    /* sx1262_set_dio_input_enable/sx1262_get_dio_input_enable test */
    sx1262_interface_debug_print("sx1262: sx1262_set_dio_input_enable/sx1262_get_dio_input_enable test.\n");
    
    res = sx1262_set_dio_output_enable(&gs_handle, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio output enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    enable = rand() % 0xF;
    res = sx1262_set_dio_input_enable(&gs_handle, enable);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio input enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set dio input enable 0x%02X.\n", enable);
    res = sx1262_get_dio_input_enable(&gs_handle, (uint8_t *)&enable_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get dio input enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check dio input enable %s.\n", enable == enable_check ? "ok" : "error");
    
    /* sx1262_set_pull_up_control/sx1262_get_pull_up_control test */
    sx1262_interface_debug_print("sx1262: sx1262_set_pull_up_control/sx1262_get_pull_up_control test.\n");
    
    control = rand() % 0x0F;
    res = sx1262_set_pull_up_control(&gs_handle, control);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pull up control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set pull up control 0x%02X.\n", control);
    res = sx1262_get_pull_up_control(&gs_handle, (uint8_t *)&control_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get pull up control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check pull up control %s.\n", control == control_check ? "ok" : "error");
    
    /* sx1262_set_pull_down_control/sx1262_get_pull_down_control test */
    sx1262_interface_debug_print("sx1262: sx1262_set_pull_down_control/sx1262_get_pull_down_control test.\n");
    
    res = sx1262_set_pull_up_control(&gs_handle, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pull up control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    control = rand() % 0x0F;
    res = sx1262_set_pull_down_control(&gs_handle, control);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pull down control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set pull down control 0x%02X.\n", control);
    res = sx1262_get_pull_down_control(&gs_handle, (uint8_t *)&control_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get pull down control failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check pull down control %s.\n", control == control_check ? "ok" : "error");
    
    /* sx1262_set_fhss_hopping_enable/sx1262_get_fhss_hopping_enable test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_hopping_enable/sx1262_get_fhss_hopping_enable test.\n");
    
    /* enable fhss hopping */
    res = sx1262_set_fhss_hopping_enable(&gs_handle, 0x01);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss hopping enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: enable fhss hopping.\n");
    res = sx1262_get_fhss_hopping_enable(&gs_handle, &v_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss hopping enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss hopping enable %s.\n", v_check == 0x01 ? "ok" : "error");
    
    /* disable fhss hopping */
    res = sx1262_set_fhss_hopping_enable(&gs_handle, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss hopping enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: disable fhss hopping.\n");
    res = sx1262_get_fhss_hopping_enable(&gs_handle, &v_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss hopping enable failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss hopping enable %s.\n", v_check == 0x00 ? "ok" : "error");
    
    /* sx1262_set_fhss_packet_length/sx1262_get_fhss_packet_length test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_packet_length/sx1262_get_fhss_packet_length test.\n");
    
    v = rand() % 16;
    res = sx1262_set_fhss_packet_length(&gs_handle, v);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss packet length failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss packet length %d.\n", v);
    res = sx1262_get_fhss_packet_length(&gs_handle, &v_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss packet length failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss packet length %s.\n", v == v_check ? "ok" : "error");
    
    /* sx1262_set_fhss_nb_hopping_blocks/sx1262_get_fhss_nb_hopping_blocks test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_nb_hopping_blocks/sx1262_get_fhss_nb_hopping_blocks test.\n");
    
    v = rand() % 16;
    res = sx1262_set_fhss_nb_hopping_blocks(&gs_handle, v);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss nb hopping blocks failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss nb hopping blocks %d.\n", v);
    res = sx1262_get_fhss_nb_hopping_blocks(&gs_handle, &v_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss nb hopping blocks failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss nb hopping blocks %s.\n", v == v_check ? "ok" : "error");
    
    /* sx1262_set_fhss_symbols_freq0/sx1262_get_fhss_symbols_freq0 test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_symbols_freq0/sx1262_get_fhss_symbols_freq0 test.\n");
    
    value = rand() % 0xFFFFUL;
    res = sx1262_set_fhss_symbols_freq0(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss symbols freq0 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss symbols freq0 %d.\n", value);
    res = sx1262_get_fhss_symbols_freq0(&gs_handle, &value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss symbols freq0 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss symbols freq0 %s.\n", value == value_check ? "ok" : "error");
    
    /* sx1262_set_fhss_freq0/sx1262_get_fhss_freq0 test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_freq0/sx1262_get_fhss_freq0 test.\n");
    
    freq = rand() % 0xFFFFUL;
    res = sx1262_set_fhss_freq0(&gs_handle, freq);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss freq0 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss freq0 %d.\n", freq);
    res = sx1262_get_fhss_freq0(&gs_handle, &freq_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss freq0 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss freq0 %s.\n", freq == freq_check ? "ok" : "error");
    
    /* sx1262_set_fhss_symbols_freq15/sx1262_get_fhss_symbols_freq15 test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_symbols_freq15/sx1262_get_fhss_symbols_freq15 test.\n");
    
    value = rand() % 0xFFFFUL;
    res = sx1262_set_fhss_symbols_freq15(&gs_handle, value);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss symbols freq15 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss symbols freq15 %d.\n", value);
    res = sx1262_get_fhss_symbols_freq15(&gs_handle, &value_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss symbols freq15 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss symbols freq15 %s.\n", value == value_check ? "ok" : "error");
    
    /* sx1262_set_fhss_freq15/sx1262_get_fhss_freq15 test */
    sx1262_interface_debug_print("sx1262: sx1262_set_fhss_freq15/sx1262_get_fhss_freq15 test.\n");
    
    freq = rand() % 0xFFFFUL;
    res = sx1262_set_fhss_freq15(&gs_handle, freq);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set fhss freq15 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: set fhss freq15 %d.\n", freq);
    res = sx1262_get_fhss_freq15(&gs_handle, &freq_check);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get fhss freq15 failed.\n");
        (void)sx1262_deinit(&gs_handle);
        
        return 1;
    }
    sx1262_interface_debug_print("sx1262: check fhss freq15 %s.\n", freq == freq_check ? "ok" : "error");
    
    /* finish register test */
    sx1262_interface_debug_print("sx1262: finish register test.\n");
    (void)sx1262_deinit(&gs_handle);
    
    return 0;
}
