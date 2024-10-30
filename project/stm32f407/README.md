### 1. Chip

#### 1.1 Chip Info

Chip Name: STM32F407ZGT6.

Extern Oscillator: 8MHz.

UART Pin: TX/RX PA9/PA10.

SPI Pin: SCK/MISO/MOSI/CS  PA5/PA6/PA7/PA4.

RESET Pin: RST PB1.

DIO1 Pin: DIO1 PB0.

BUSY Pin: BUSY PA8.

### 2. Development and Debugging

#### 2.1 Integrated Development Environment

LibDriver provides both Keil and IAR integrated development environment projects.

MDK is the Keil ARM project and your Keil version must be 5 or higher.Keil ARM project needs STMicroelectronics STM32F4 Series Device Family Pack and you can download from https://www.keil.com/dd2/stmicroelectronics/stm32f407zgtx.

EW is the IAR ARM project and your IAR version must be 9 or higher.

#### 2.2 Serial Port Parameter

Baud Rate: 115200.

Data Bits : 8.

Stop Bits: 1.

Parity: None.

Flow Control: None.

#### 2.3 Serial Port Assistant

We use '\n' to wrap lines.If your serial port assistant displays exceptions (e.g. the displayed content does not divide lines), please modify the configuration of your serial port assistant or replace one that supports '\n' parsing.

### 3. SX1262

#### 3.1 Command Instruction

1. Show sx1262 chip and driver information.

   ```shell
   sx1262 (-i | --information)
   ```

2. Show sx1262 help.

   ```shell
   sx1262 (-h | --help)
   ```

3. Show sx1262 pin connections of the current board.

   ```shell
   sx1262 (-p | --port)
   ```

4. Run sx1262 register test.

   ```shell
   sx1262 (-t reg | --test=reg)
   ```

5. Run sx1262 send test.

   ```shell
   sx1262 (-t lora-send | --test=lora-send)
   ```

6. Run sx1262 receive test, time is the received timeout in second. 

   ```shell
   sx1262 (-t lora-receive | --test=lora-receive) [--timeout=<time>]
   ```

7. Run sx1262 cad test.

   ```shell
   sx1262 (-t lora-cad | --test=lora-cad)
   ```

8. Run sx1262  send function, str is the send data.

   ```shell
   sx1262 (-e lora-send | --example=lora-send) [--data=<str>]
   ```

9. Run sx1262 receive function, time is the received timeout in second. 

   ```shell
   sx1262 (-e lora-receive | --example=lora-receive) [--timeout=<time>]
   ```

10. Run sx1262 cad function.

    ```shell
    sx1262 (-e lora-cad | --example=lora-cad)
    ```

11. Run sx1262 sleep function.

    ```shell
    sx1262 (-e lora-sleep | --example=lora-sleep)
    ```

12. Run sx1262 wake up function.

    ```shell
    sx1262 (-e lora-wake-up | --example=lora-wake-up)
    ```

#### 3.2 Command Example

```shell
sx1262 -i

sx1262: chip is Semtech SX1262.
sx1262: manufacturer is Semtech.
sx1262: interface is SPI.
sx1262: driver version is 1.0.
sx1262: min supply voltage is 1.8V.
sx1262: max supply voltage is 3.7V.
sx1262: max current is 107.00mA.
sx1262: max temperature is 85.0C.
sx1262: min temperature is -40.0C.
```

```shell
sx1262 -p

sx1262: SCK connected to GPIOA PIN5.
sx1262: MISO connected to GPIOA PIN6.
sx1262: MOSI connected to GPIOA PIN7.
sx1262: CS connected to GPIOA PIN4.
sx1262: RESET connected to GPIOB PIN1.
sx1262: DIO1 connected to GPIOB PIN0.
sx1262: BUSY connected to GPIOA PIN8.
```

```shell
sx1262 -t reg

sx1262: chip is Semtech SX1262.
sx1262: manufacturer is Semtech.
sx1262: interface is SPI.
sx1262: driver version is 1.0.
sx1262: min supply voltage is 1.8V.
sx1262: max supply voltage is 3.7V.
sx1262: max current is 107.00mA.
sx1262: max temperature is 85.0C.
sx1262: min temperature is -40.0C.
sx1262: start register test.
sx1262: sx1262_write_register/sx1262_read_register test.
sx1262: set register 0xB0.
sx1262: check register ok.
sx1262: sx1262_write_buffer/sx1262_read_buffer test.
sx1262: set buffer 0xCB.
sx1262: check buffer ok.
sx1262: sx1262_timeout_convert_to_register/sx1262_timeout_convert_to_data test.
sx1262: set timeout 640.900000 us.
sx1262: check timeout 640.625000 us.
sx1262: sx1262_frequency_convert_to_register/sx1262_frequency_convert_to_data test.
sx1262: set frequency 47000000 Hz.
sx1262: check frequency 47000000 Hz.
sx1262: sx1262_gfsk_bit_rate_convert_to_register/sx1262_gfsk_bit_rate_convert_to_data test.
sx1262: set bit rate 470000 Hz.
sx1262: check bit rate 470156 Hz.
sx1262: sx1262_gfsk_frequency_deviation_convert_to_register/sx1262_gfsk_frequency_deviation_convert_to_data test.
sx1262: gfsk frequency deviation 10000 Hz.
sx1262: check gfsk frequency deviation 9999 Hz.
sx1262: sx1262_set_packet_type/sx1262_get_packet_type test.
sx1262: set gfsk type.
sx1262: check packet type ok.
sx1262: set lora type.
sx1262: check packet type ok.
sx1262: sx1262_set_fsk_whitening_initial_value/sx1262_get_fsk_whitening_initial_value test.
sx1262: set fsk whitening initial value 0x0100.
sx1262: check fsk whitening initial value ok.
sx1262: sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test.
sx1262: set fsk crc initical value 0xD868.
sx1262: check fsk crc initical value ok.
sx1262: sx1262_set_fsk_crc_initical_value/sx1262_get_fsk_crc_initical_value test.
sx1262: set fsk crc polynomial value 0xAD07.
sx1262: check fsk crc polynomial value ok.
sx1262: sx1262_set_fsk_sync_word/sx1262_get_fsk_sync_word test.
sx1262: set fsk sync word 0x45 0xC8 0x7F 0x13 0xC4 0x6C 0x2B 0xBA.
sx1262: check fsk sync word ok.
sx1262: sx1262_set_fsk_node_address/sx1262_get_fsk_node_address test.
sx1262: set fsk node address 0x61.
sx1262: check fsk node address ok.
sx1262: sx1262_set_fsk_broadcast_address/sx1262_get_fsk_broadcast_address test.
sx1262: set fsk broadcast address 0x65.
sx1262: check fsk broadcast address ok.
sx1262: sx1262_set_iq_polarity/sx1262_get_iq_polarity test.
sx1262: set iq polarity 0xA1.
sx1262: check iq polarity ok.
sx1262: sx1262_set_lora_sync_word/sx1262_get_lora_sync_word test.
sx1262: set lora sync word 0xAC72.
sx1262: check lora sync word ok.
sx1262: sx1262_get_random_number test.
sx1262: random number is 0x2C946522.
sx1262: sx1262_set_tx_modulation/sx1262_get_tx_modulation test.
sx1262: set tx modulation 0x04.
sx1262: check tx modulation ok.
sx1262: sx1262_set_rx_gain/sx1262_get_rx_gain test.
sx1262: set rx gain 0x64.
sx1262: check rx gain ok.
sx1262: sx1262_set_tx_clamp_config/sx1262_get_tx_clamp_config test.
sx1262: set tx clamp config 0x39.
sx1262: check tx clamp config ok.
sx1262: sx1262_set_ocp/sx1262_get_ocp test.
sx1262: set ocp 0x1D.
sx1262: check ocp config ok.
sx1262: sx1262_set_rtc_control/sx1262_get_rtc_control test.
sx1262: disable rtc control.
sx1262: check rtc control ok.
sx1262: sx1262_set_xta_trim/sx1262_get_xta_trim test.
sx1262: set xta trim 0x05.
sx1262: check xta trim ok.
sx1262: sx1262_set_xtb_trim/sx1262_get_xtb_trim test.
sx1262: set xtb trim 0x05.
sx1262: check xtb trim ok.
sx1262: sx1262_set_dio3_output_control/sx1262_get_dio3_output_control test.
sx1262: set dio3 output control 0x01.
sx1262: check dio3 output control ok.
sx1262: sx1262_set_event_mask/sx1262_get_event_mask test.
sx1262: set event mask 0x00.
sx1262: check event mask ok.
sx1262: sx1262_set_dio_output_enable/sx1262_get_dio_output_enable test.
sx1262: set dio output enable 0x00.
sx1262: check dio output enable ok.
sx1262: sx1262_set_dio_input_enable/sx1262_get_dio_input_enable test.
sx1262: set dio input enable 0x07.
sx1262: check dio input enable ok.
sx1262: sx1262_set_pull_up_control/sx1262_get_pull_up_control test.
sx1262: set pull up control 0x0A.
sx1262: check pull up control ok.
sx1262: sx1262_set_pull_down_control/sx1262_get_pull_down_control test.
sx1262: set pull down control 0x05.
sx1262: check pull down control ok.
sx1262: sx1262_set_fhss_hopping_enable/sx1262_get_fhss_hopping_enable test.
sx1262: enable fhss hopping.
sx1262: check fhss hopping enable ok.
sx1262: disable fhss hopping.
sx1262: check fhss hopping enable ok.
sx1262: sx1262_set_fhss_packet_length/sx1262_get_fhss_packet_length test.
sx1262: set fhss packet length 9.
sx1262: check fhss packet length ok.
sx1262: sx1262_set_fhss_nb_hopping_blocks/sx1262_get_fhss_nb_hopping_blocks test.
sx1262: set fhss nb hopping blocks 8.
sx1262: check fhss nb hopping blocks ok.
sx1262: sx1262_set_fhss_symbols_freq0/sx1262_get_fhss_symbols_freq0 test.
sx1262: set fhss symbols freq0 20312.
sx1262: check fhss symbols freq0 ok.
sx1262: sx1262_set_fhss_freq0/sx1262_get_fhss_freq0 test.
sx1262: set fhss freq0 30807.
sx1262: check fhss freq0 ok.
sx1262: sx1262_set_fhss_symbols_freq15/sx1262_get_fhss_symbols_freq15 test.
sx1262: set fhss symbols freq15 54613.
sx1262: check fhss symbols freq15 ok.
sx1262: sx1262_set_fhss_freq15/sx1262_get_fhss_freq15 test.
sx1262: set fhss freq15 59872.
sx1262: check fhss freq15 ok.
sx1262: finish register test.
```

```shell
sx1262 -t lora-send

sx1262: start send test.
sx1262: irq tx done.
sx1262: finish send test.
```

```shell
sx1262 -t lora-receive --timeout=1000

sx1262: start receive test.
sx1262: start receiving...
sx1262: irq preamble detected.
sx1262: irq valid header.
sx1262: irq rx done.
sx1262: rssi is -110.0.
sx1262: snr is 63.75.
sx1262: signal rssi is -113.0.
0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1A 0x1B 0x1C 0x1D 0x1E 0x1F 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2A 0x2B 0x2C 0x2D 0x2E 0x2F 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3A 0x3B 0x3C 0x3D 0x3E 0x3F 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4A 0x4B 0x4C 0x4D 0x4E 0x4F 0x50 0x51 0x52 0x53 0x54 0x55 0x56 0x57 0x58 0x59 0x5A 0x5B 0x5C 0x5D 0x5E 0x5F 0x60 0x61 0x62 0x63 0x64 0x65 0x66 0x67 0x68 0x69 0x6A 0x6B 0x6C 0x6D 0x6E 0x6F 0x70 0x71 0x72 0x73 0x74 0x75 0x76 0x77 0x78 0x79 0x7A 0x7B 0x7C 0x7D 0x7E 0x7F 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8A 0x8B 0x8C 0x8D 0x8E 0x8F 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9A 0x9B 0x9C 0x9D 0x9E 0x9F 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 0xA6 0xA7 0xA8 0xA9 0xAA 0xAB 0xAC 0xAD 0xAE 0xAF 0xB0 0xB1 0xB2 0xB3 0xB4 0xB5 0xB6 0xB7 0xB8 0xB9 0xBA 0xBB 0xBC 0xBD 0xBE 0xBF 
sx1262: finish receive test.
```

```shell
sx1262 -t lora-cad

sx1262: start cad test.
sx1262: irq cad done.
sx1262: irq cad detected.
sx1262: cad detected.
sx1262: finish cad test.
```

```shell
sx1262 -e lora-send --data=LibDriver

sx1262: send LibDriver.
sx1262: irq tx done.
```

```shell
sx1262 -e lora-receive --timeout=1000

sx1262: start receiving...
sx1262: irq rx done.
sx1262: rssi is -110.0.
sx1262: snr is 62.25.
LibDriver
```

```shell
sx1262 -e lora-cad

sx1262: irq cad done.
sx1262: cad not detected.
```

```shell
sx1262 -e lora-sleep

sx1262: sleep.
```

```shell
sx1262 -e lora-wake-up

sx1262: wake up.
```

```shell
sx1262 -h

Usage:
  sx1262 (-i | --information)
  sx1262 (-h | --help)
  sx1262 (-p | --port)
  sx1262 (-t reg | --test=reg)
  sx1262 (-t lora-send | --test=lora-send)
  sx1262 (-t lora-receive | --test=lora-receive) [--timeout=<time>]
  sx1262 (-t lora-cad | --test=lora-cad)
  sx1262 (-e lora-send | --example=lora-send) [--data=<str>]
  sx1262 (-e lora-receive | --example=lora-receive) [--timeout=<time>]
  sx1262 (-e lora-cad | --example=lora-cad)
  sx1262 (-e lora-sleep | --example=lora-sleep)
  sx1262 (-e lora-wake-up | --example=lora-wake-up)

Options:
      --data=<str>            Set the send data.([default: LibDriver])
  -e <lora-send | lora-receive | lora-cad | lora-sleep | lora-wake-up>, --example=<lora-send
     | lora-receive | lora-cad | lora-sleep | lora-wake-up>
                              Run the driver example.
  -h, --help                  Show the help.
  -i, --information           Show the chip information.
  -p, --port                  Display the pin connections of the current board.
  -t <reg | lora-send | lora-receive | lora-cad>, --test=<reg | lora-send | lora-receive | lora-cad>
                              Run the driver test.
      --timeout=<time>        Set the timeout in ms.([default: 1000])
```

