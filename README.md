# iRadioPico für Raspberry Pico / Pico W (rp2040)

Ein Softwarebaukasten für den Aufbau neuer Radios oder Umbau alter Radios zu einem Internetradio mit Raspberry Pico und Pico W auf Arduino Basis.

Das iRadioPico ist eine weitere Portierung des iRadio-Baukastens, welchen es bereits für verschiede Plattformen gibt:

* für ESP32 https://github.com/BM45/iRadioMini
* für Raspberry SoC https://github.com/BM45/iRadio
* für Android https://github.com/BM45/iRadioAndroid

## Unterstützte Boards:

Das iRadioPico wurde primär für Raspberry Pico und Pico W (die Wifi-Variante) entwickelt 

![picos](https://github.com/BM45/iRadioPico/blob/main/pics4www/four_picos.jpg)

unterstützt aber auch die vielen anderen Nachbauten und Boards mit der RP2040 MCU.

## Steuerung des iRadioPico durch:

Programmumschaltung zum Beispiel mit (Micro-)Taster, Inkrementaldrehgeber, Drehimpulsgeber oder Gestenerkennung. Weitere Steuerungsszenarios sind denkbar.

Diese Steuerung wird wie in den anderen iRadio Baukästen als "gpiod" Daemon/Prozess bezeichnet.

## Unterstützte Displays für Nutzerschnittstelle (displayd):
Democode vorhanden für:
- HD44780-kompatible Displays (auch über I2C mit PCF8574/5)

![HD44780](https://github.com/BM45/iRadioPico/blob/main/pics4www/i2clcd.jpg)

- über U8g2 (U8g2 is a monochrome graphics library for embedded devices.)

 SSD1305, SSD1306, SSD1309, SSD1316, SSD1322, SSD1325, SSD1327, SSD1329, SSD1606, SSD1607, SH1106, SH1107, SH1108, SH1122, T6963, RA8835, LC7981, PCD8544, PCF8812, HX1230, UC1601, UC1604, UC1608, UC1610, UC1611, UC1617, UC1701, ST7511, ST7528, ST7565, ST7567, ST7571, ST7586, ST7588, ST75256, ST75320, NT7534, ST7920, IST3020, IST7920, LD7032, KS0108, KS0713, SED1520, SBN1661, IL3820, MAX7219 (siehe https://github.com/olikraus/u8g2/wiki/u8g2setupcpp für eine vollständige Liste)

![u8g2](https://github.com/BM45/iRadioPico/blob/main/pics4www/ssd1306.jpg)

- weitere zahlreiche Controller über Arduino_GFX lib
  siehe https://github.com/moononournation/Arduino_GFX#currently-supported-display-wiki

![arduinogfx](https://github.com/BM45/iRadioPico/blob/main/pics4www/st7735.jpg)

  Durch die Einbettung des iRadioPico ins Arduino-Ökosystems stehen viele weitere Bibliothek zur Ansteuerung von Displays zur Verfügung

Das besonderes Highlight, die Simulation photorealistischer Nachbildungen von Senderskalen alter Radios, ist auch beim iRadioPico vertreten.

![sim](https://github.com/BM45/iRadioPico/blob/main/pics4www/babymetz.jpg)

## Unterstützte Audio-DACs:

Standardmäßig ist zur Zeit die Ausgabe über den VS1053 Dekoder DSP vorgesehen, der Player ist jedoch für zahlreiche Softwarecodec und I2S-DACs (Hardware/Software) offen.
Tests mit den "AudioTools" von Phil Schatzmann https://www.pschatzmann.ch/home/2023/02/28/arduino-audiotools-esp32-vs-rp2040-pico-w/ wie auch einer angepassten Version der ESP8266AudioLib von Earle F. Philhower https://github.com/earlephilhower/ESP8266Audio verliefen sehr erfolgreich! 
Die Entwicklung auf dem Gebiet der Softwarecodec ist im Moment sehr dynamisch, speziell für den RP2040, deshalb haben wir uns noch nicht für den einen Lieblingscodec entschieden! Da der Pico mit seinem PIO-System sehr gut solche Schnittstellen und DACs darstellen kann, ist das iRadioPico eine tolle Spielwiese für eigenen Experimente in diese Richtungen. 

https://github.com/malacalypse/rp2040_i2s_example 
https://www.elektronik-labor.de/Raspberry/Pico13.html
