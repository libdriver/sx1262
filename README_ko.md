[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver SX1262

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/sx1262/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

SX1262 sub-GHz 무선 트랜시버는 장거리 무선 애플리케이션에 이상적입니다. 두 장치 모두 활성 수신 전류 소모가 4.2mA에 불과하여 배터리 수명이 길도록 설계되었습니다. SX1261은 최대 +15dBm까지 전송할 수 있고 SX1262와 SX1268은 고효율 통합 전력 증폭기로 최대 +22dBm까지 전송할 수 있습니다. 이 장치는 LPWAN 사용 사례에 대한 LoRa® 및 장거리 FHSS 변조와 레거시 사용 사례에 대한 (G)FSK 변조를 지원합니다. 이 장치는 글로벌 LoRaWAN® 표준 또는 독점 프로토콜을 활용하여 다양한 애플리케이션 요구 사항을 충족하도록 매우 구성 가능합니다. 이 장치는 LoRa Alliance®에서 발표한 LoRaWAN 사양의 물리적 계층 요구 사항을 준수하도록 설계되었습니다. 이 무선은 ETSI EN 300 220, FCC CFR 47 Part 15, 중국 규정 요구 사항 및 일본 ARIB T-108을 포함하되 이에 국한되지 않는 무선 규정 준수를 목표로 하는 시스템에 적합합니다. 150MHz에서 960MHz까지의 연속 주파수 범위를 통해 전 세계 모든 주요 sub-GHz ISM 대역을 지원할 수 있습니다.

LibDriver SX1262은 LibDriver에서 출시한 SX1262의 전체 기능 드라이버입니다. 무선 전송, 무선 수신, cad 등의 기능을 제공합니다. LibDriver는 MISRA를 준수합니다.

### 콘텐츠

  - [설명](#설명)
  - [설치](#설치)
  - [사용](#사용)
    - [example basic send](#example-basic-send)
    - [example basic receive](#example-basic-receive)
  - [문서](#문서)
  - [기고](#기고)
  - [저작권](#저작권)
  - [문의하기](#문의하기)

### 설명

/src 디렉토리에는 LibDriver SX1262의 소스 파일이 포함되어 있습니다.

/interface 디렉토리에는 LibDriver SX1262용 플랫폼 독립적인 SPI버스 템플릿이 포함되어 있습니다.

/test 디렉토리에는 LibDriver SX1262드라이버 테스트 프로그램이 포함되어 있어 칩의 필요한 기능을 간단히 테스트할 수 있습니다.

/example 디렉토리에는 LibDriver SX1262프로그래밍 예제가 포함되어 있습니다.

/doc 디렉토리에는 LibDriver SX1262오프라인 문서가 포함되어 있습니다.

/datasheet 디렉토리에는 SX1262데이터시트가 있습니다.

/project 디렉토리에는 일반적으로 사용되는 Linux 및 마이크로컨트롤러 개발 보드의 프로젝트 샘플이 포함되어 있습니다. 모든 프로젝트는 디버깅 방법으로 셸 스크립트를 사용하며, 자세한 내용은 각 프로젝트의 README.md를 참조하십시오.

/misra 에는 LibDriver misra 코드 검색 결과가 포함됩니다.

### 설치

/interface 디렉토리에서 플랫폼 독립적인SPI 버스 템플릿을 참조하여 지정된 플랫폼에 대한 SPI버스 드라이버를 완성하십시오.

/src 디렉터리, 플랫폼용 인터페이스 드라이버 및 자체 드라이버를 프로젝트에 추가합니다. 기본 예제 드라이버를 사용하려면 /example 디렉터리를 프로젝트에 추가합니다.

### 사용

/example 디렉터리의 예제를 참조하여 자신만의 드라이버를 완성할 수 있습니다. 기본 프로그래밍 예제를 사용하려는 경우 사용 방법은 다음과 같습니다.

#### example basic send

```C
#include "driver_sx1262_lora.h"

uint8_t (*g_gpio_irq)(void) = NULL;
uint8_t res;
static uint8_t gs_rx_done;

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
            break;
        }
    }
}

/* gpio init */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}
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

sx1262_interface_debug_print("sx1262: send %s.\n", "123");

/* send data */
res = sx1262_lora_send((uint8_t *)"123", strlen("123"));
if (res != 0)
{
    (void)sx1262_lora_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}

/* deinit */
res = sx1262_lora_deinit();
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

#### example basic receive

```c
#include "driver_sx1262_lora.h"

uint8_t (*g_gpio_irq)(void) = NULL;
uint8_t res;
uint32_t timeout;
static uint8_t gs_rx_done;

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
                return 1;
            }
            sx1262_interface_debug_print("sx1262: rssi is %0.1f.\n", rssi);
            sx1262_interface_debug_print("sx1262: snr is %0.2f.\n", snr);
            
            /* check the error */
            if (sx1262_lora_check_packet_error(&enable) != 0)
            {
                return 1;
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
            break;
        }
    }
}

/* gpio init */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}
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
timeout = 3000;

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

/* deinit */
res = sx1262_lora_deinit();
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 문서

온라인 문서: [https://www.libdriver.com/docs/sx1262/index.html](https://www.libdriver.com/docs/sx1262/index.html).

오프라인 문서: /doc/html/index.html.

### 기고

CONTRIBUTING.md 를 참조하십시오.

### 저작권

저작권 (c) 2015 - 지금 LibDriver 판권 소유

MIT 라이선스(MIT)

이 소프트웨어 및 관련 문서 파일("소프트웨어")의 사본을 얻은 모든 사람은 이에 따라 무제한 사용, 복제, 수정, 통합, 출판, 배포, 2차 라이선스를 포함하여 소프트웨어를 처분할 수 있는 권리가 부여됩니다. 소프트웨어의 사본에 대한 라이선스 및/또는 판매, 그리고 소프트웨어가 위와 같이 배포된 사람의 권리에 대한 2차 라이선스는 다음 조건에 따릅니다.

위의 저작권 표시 및 이 허가 표시는 이 소프트웨어의 모든 사본 또는 내용에 포함됩니다.

이 소프트웨어는 상품성, 특정 목적에의 적합성 및 비침해에 대한 보증을 포함하되 이에 국한되지 않는 어떠한 종류의 명시적 또는 묵시적 보증 없이 "있는 그대로" 제공됩니다. 어떤 경우에도 저자 또는 저작권 소유자는 계약, 불법 행위 또는 기타 방식에 관계없이 소프트웨어 및 기타 소프트웨어 사용으로 인해 발생하거나 이와 관련하여 발생하는 청구, 손해 또는 기타 책임에 대해 책임을 지지 않습니다.

### 문의하기

연락주세요lishifenging@outlook.com.