[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver SX1262

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/sx1262/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

SX1262-Sub-GHz-Funktransceiver sind ideal für drahtlose Anwendungen mit großer Reichweite. Beide Geräte sind für eine lange Batterielebensdauer mit nur 4,2 mA aktivem Empfangsstromverbrauch ausgelegt. Der SX1261 kann bis zu +15 dBm übertragen und der SX1262 und SX1268 können mit hocheffizienten integrierten Leistungsverstärkern bis zu +22 dBm übertragen. Diese Geräte unterstützen LoRa®- und Long Range FHSS-Modulationen für LPWAN-Anwendungsfälle und (G)FSK-Modulation für Legacy-Anwendungsfälle. Die Geräte sind hochgradig konfigurierbar, um unterschiedliche Anwendungsanforderungen unter Verwendung des globalen LoRaWAN®-Standards oder proprietärer Protokolle zu erfüllen. Die Geräte sind so konzipiert, dass sie die Anforderungen der physischen Schicht der von der LoRa Alliance® veröffentlichten LoRaWAN-Spezifikation erfüllen. Das Funkgerät ist für Systeme geeignet, die die Einhaltung von Funkvorschriften anstreben, einschließlich, aber nicht beschränkt auf ETSI EN 300 220, FCC CFR 47 Teil 15, chinesische behördliche Vorschriften und das japanische ARIB T-108. Die kontinuierliche Frequenzabdeckung von 150 MHz bis 960 MHz ermöglicht die Unterstützung aller wichtigen Sub-GHz-ISM-Bänder weltweit.

LibDriver SX1262 ist der voll funktionsfähige Treiber von SX1262, der von LibDriver gestartet wurde. Er bietet Funktionen für drahtloses Senden, drahtloses Empfangen, CAD usw. LibDriver ist MISRA-konform.

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example basic send](#example-basic-send)
    - [example basic receive](#example-basic-receive)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver SX1262-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver SX1262 SPI.

/test enthält den Testcode des LibDriver SX1262-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen.

/example enthält LibDriver SX1262-Beispielcode.

/doc enthält das LibDriver SX1262-Offlinedokument.

/Datenblatt enthält SX1262-Datenblatt.

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

/misra enthält die Ergebnisse des LibDriver MISRA Code Scans.

### Installieren

Verweisen Sie auf eine plattformunabhängige SPI-Schnittstellenvorlage und stellen Sie Ihren Plattform-SPI-Treiber fertig.

Fügen Sie das Verzeichnis /src, den Schnittstellentreiber für Ihre Plattform und Ihre eigenen Treiber zu Ihrem Projekt hinzu. Wenn Sie die Standardbeispieltreiber verwenden möchten, fügen Sie das Verzeichnis /example zu Ihrem Projekt hinzu.

### Nutzung

Sie können auf die Beispiele im Verzeichnis /example zurückgreifen, um Ihren eigenen Treiber zu vervollständigen. Wenn Sie die Standardprogrammierbeispiele verwenden möchten, erfahren Sie hier, wie Sie diese verwenden.

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

### Dokument

Online-Dokumente: [https://www.libdriver.com/docs/sx1262/index.html](https://www.libdriver.com/docs/sx1262/index.html).

Offline-Dokumente: /doc/html/index.html.

### Beitrag

Bitte beachten Sie CONTRIBUTING.md.

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesendliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com.