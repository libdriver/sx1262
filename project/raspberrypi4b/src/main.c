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
 * @file      main.c
 * @brief     main source file
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
#include "driver_sx1262_send_receive_test.h"
#include "driver_sx1262_cad_test.h"
#include "driver_sx1262_lora.h"
#include "gpio.h"
#include <getopt.h>
#include <stdlib.h>

uint8_t (*g_gpio_irq)(void) = NULL;        /**< gpio irq function address */
static volatile uint8_t gs_rx_done;        /**< rx done */

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
            float rssi;
            float snr;
            
            sx1262_interface_debug_print("sx1262: irq rx done.\n");
            
            /* get the status */
            if (sx1262_lora_get_status((float *)&rssi, (float *)&snr) != 0)
            {
                return;
            }
            sx1262_interface_debug_print("sx1262: rssi is %0.1f.\n", rssi);
            sx1262_interface_debug_print("sx1262: snr is %0.2f.\n", snr);
            
            /* check the error */
            if (sx1262_lora_check_packet_error(&enable) != 0)
            {
                return;
            }
            if ((enable == SX1262_BOOL_FALSE) && len)
            {
                for (i = 0; i < len; i++)
                {
                    sx1262_interface_debug_print("%c", buf[i]);
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
            sx1262_interface_debug_print("sx1262: unknown code.\n");
            
            break;
        }
    }
}

/**
 * @brief     sx1262 full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 *             - 1 run failed
 *             - 5 param is invalid
 * @note      none
 */
uint8_t sx1262(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"data", required_argument, NULL, 1},
        {"timeout", required_argument, NULL, 2},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    char data[33] = "LibDriver";
    uint32_t timeout = 1000;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* data */
            case 1 :
            {
                /* set the data */
                memset(data, 0, sizeof(char) * 33);
                strncpy(data, optarg, 32);

                break;
            }
            
            /* timeout */
            case 2 :
            {
                /* set the timeout */
                timeout = atol(optarg);
                
                break;
            } 
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_reg", type) == 0)
    {
        uint8_t res;
        
        /* run reg test */
        res = sx1262_register_test();
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_lora-send", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_interrupt_test_irq_handler;
        
        /* run send test */
        res = sx1262_send_test();
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("t_lora-receive", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_interrupt_test_irq_handler;

        /* run receive test */
        res = sx1262_receive_test(timeout);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("t_lora-cad", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_cad_test_irq_handler;

        /* run cad test */
        res = sx1262_cad_test();
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_lora-send", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_lora_irq_handler;
        
        /* lora init */
        res = sx1262_lora_init(a_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* set send mode */
        res = sx1262_lora_set_send_mode();
        if (res != 0)
        {
            (void)sx1262_lora_deinit();
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* output */
        sx1262_interface_debug_print("sx1262: send %s.\n", data);
        
        /* send data */
        res = sx1262_lora_send((uint8_t *)data, (uint16_t)strlen(data));
        if (res != 0)
        {
            (void)sx1262_lora_deinit();
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* lora deinit */
        res = sx1262_lora_deinit();
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_lora-receive", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_lora_irq_handler;
        
        /* lora init */
        res = sx1262_lora_init(a_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* start receiving */
        sx1262_interface_debug_print("sx1262: start receiving...\n");
        gs_rx_done = 0;

        /* start receive */
        res = sx1262_lora_set_continuous_receive_mode();
        if (res != 0)
        {
            (void)sx1262_lora_deinit();
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        while ((timeout != 0) && (gs_rx_done == 0))
        {
            timeout--;
            sx1262_interface_delay_ms(1000);
        }
        if (gs_rx_done == 0)
        {
            /* receive timeout */
            sx1262_interface_debug_print("sx1262: receive timeout.\n");
            (void)sx1262_lora_deinit();
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* lora deinit */
        res = sx1262_lora_deinit();
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_lora-cad", type) == 0)
    {
        uint8_t res;
        sx1262_bool_t enable;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }

        /* set gpio irq */
        g_gpio_irq = sx1262_lora_irq_handler;
        
        /* lora init */
        res = sx1262_lora_init(a_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* run cad */
        res = sx1262_lora_run_cad(&enable);
        if (res != 0)
        {
            (void)sx1262_lora_deinit();
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* output the result */
        if (enable == SX1262_BOOL_TRUE)
        {
            sx1262_interface_debug_print("sx1262: cad detected.\n");
        }
        else
        {
            sx1262_interface_debug_print("sx1262: cad not detected.\n");
        }
        
        /* lora deinit */
        res = sx1262_lora_deinit();
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }

        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_lora-sleep", type) == 0)
    {
        uint8_t res;
        
        /* lora init */
        res = sx1262_lora_init(a_callback);
        if (res != 0)
        {
            return 1;
        }
        
        /* sleep */
        if (sx1262_lora_sleep() != 0)
        {
            return 1;
        }
        
        /* output */
        sx1262_interface_debug_print("sx1262: sleep.\n");
        
        return 0;
    }
    else if (strcmp("e_lora-wake-up", type) == 0)
    {
        uint8_t res;
        
        /* lora init */
        res = sx1262_lora_init(a_callback);
        if (res != 0)
        {
            return 1;
        }
        
        /* wake up */
        if (sx1262_lora_wake_up() != 0)
        {
            return 1;
        }
        
        /* output */
        sx1262_interface_debug_print("sx1262: wake up.\n");
        
        /* deinit */
        res = sx1262_lora_deinit();
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        sx1262_interface_debug_print("Usage:\n");
        sx1262_interface_debug_print("  sx1262 (-i | --information)\n");
        sx1262_interface_debug_print("  sx1262 (-h | --help)\n");
        sx1262_interface_debug_print("  sx1262 (-p | --port)\n");
        sx1262_interface_debug_print("  sx1262 (-t reg | --test=reg)\n");
        sx1262_interface_debug_print("  sx1262 (-t lora-send | --test=lora-send)\n");
        sx1262_interface_debug_print("  sx1262 (-t lora-receive | --test=lora-receive) [--timeout=<time>]\n");
        sx1262_interface_debug_print("  sx1262 (-t lora-cad | --test=lora-cad)\n");
        sx1262_interface_debug_print("  sx1262 (-e lora-send | --example=lora-send) [--data=<str>]\n");
        sx1262_interface_debug_print("  sx1262 (-e lora-receive | --example=lora-receive) [--timeout=<time>]\n");
        sx1262_interface_debug_print("  sx1262 (-e lora-cad | --example=lora-cad)\n");
        sx1262_interface_debug_print("  sx1262 (-e lora-sleep | --example=lora-sleep)\n");
        sx1262_interface_debug_print("  sx1262 (-e lora-wake-up | --example=lora-wake-up)\n");
        sx1262_interface_debug_print("\n");
        sx1262_interface_debug_print("Options:\n");
        sx1262_interface_debug_print("      --data=<str>            Set the send data.([default: LibDriver])\n");
        sx1262_interface_debug_print("  -e <lora-send | lora-receive | lora-cad | lora-sleep | lora-wake-up>, --example=<lora-send\n");
        sx1262_interface_debug_print("     | lora-receive | lora-cad | lora-sleep | lora-wake-up>\n");
        sx1262_interface_debug_print("                              Run the driver example.\n");
        sx1262_interface_debug_print("  -h, --help                  Show the help.\n");
        sx1262_interface_debug_print("  -i, --information           Show the chip information.\n");
        sx1262_interface_debug_print("  -p, --port                  Display the pin connections of the current board.\n");
        sx1262_interface_debug_print("  -t <reg | lora-send | lora-receive | lora-cad>, --test=<reg | lora-send | lora-receive | lora-cad>\n");
        sx1262_interface_debug_print("                              Run the driver test.\n");
        sx1262_interface_debug_print("      --timeout=<time>        Set the timeout in ms.([default: 1000])\n");

        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        sx1262_info_t info;
        
        /* print sx1262 info */
        sx1262_info(&info);
        sx1262_interface_debug_print("sx1262: chip is %s.\n", info.chip_name);
        sx1262_interface_debug_print("sx1262: manufacturer is %s.\n", info.manufacturer_name);
        sx1262_interface_debug_print("sx1262: interface is %s.\n", info.interface);
        sx1262_interface_debug_print("sx1262: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        sx1262_interface_debug_print("sx1262: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        sx1262_interface_debug_print("sx1262: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        sx1262_interface_debug_print("sx1262: max current is %0.2fmA.\n", info.max_current_ma);
        sx1262_interface_debug_print("sx1262: max temperature is %0.1fC.\n", info.temperature_max);
        sx1262_interface_debug_print("sx1262: min temperature is %0.1fC.\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        sx1262_interface_debug_print("sx1262: SCK connected to GPIO11(BCM).\n");
        sx1262_interface_debug_print("sx1262: MISO connected to GPIO9(BCM).\n");
        sx1262_interface_debug_print("sx1262: MOSI connected to GPIO10(BCM).\n");
        sx1262_interface_debug_print("sx1262: CS connected to GPIO8(BCM).\n");
        sx1262_interface_debug_print("sx1262: RESET connected to GPIO27(BCM).\n");
        sx1262_interface_debug_print("sx1262: DIO1 connected to GPIO22(BCM).\n");
        sx1262_interface_debug_print("sx1262: BUSY connected to GPIO17(BCM).\n");
        
        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief     main function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 * @note      none
 */
int main(uint8_t argc, char **argv)
{
    uint8_t res;

    res = sx1262(argc, argv);
    if (res == 0)
    {
        /* run success */
    }
    else if (res == 1)
    {
        sx1262_interface_debug_print("sx1262: run failed.\n");
    }
    else if (res == 5)
    {
        sx1262_interface_debug_print("sx1262: param is invalid.\n");
    }
    else
    {
        sx1262_interface_debug_print("sx1262: unknown status code.\n");
    }
    
    return 0;
}
