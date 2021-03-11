#include <TFT_eSPI.h>
#include <SPI.h>
#include "Button2.h"
#include "esp_adc_cal.h"
#include <FS.h>

// TFT Pins has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
// #define TFT_MOSI            19
// #define TFT_SCLK            18
// #define TFT_CS              5
// #define TFT_DC              16
// #define TFT_RST             23
// #define TFT_BL              4   // Display backlight control pin

#define ADC_EN 14 //ADC_EN is the ADC detection enable port
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

#define AA_FONT_SMALL "NotoSansBold15"
#define AA_FONT_LARGE "NotoSansBold36"

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

//char buff[512];
int vref = 1100;
int btnCick = false;

void showVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000)
    {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "Voltage :" + String(battery_voltage) + "V";
        Serial.println(voltage);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(voltage, tft.width() / 2, tft.height() / 2);
    }
}

void button_init()
{
    btn1.setLongClickHandler([](Button2 &btn) {
        Serial.println("Button 1 Long");
    });
    btn1.setPressedHandler([](Button2 &btn) {
        Serial.println("Button 1 Short");
        btnCick = true;
    });
    btn1.setDoubleClickHandler([](Button2 &btn) {
        Serial.println("Button 1 Dubble");
    });
    btn2.setPressedHandler([](Button2 &btn) {
        btnCick = false;
        Serial.println("Button 2 Short");
    });
    btn2.setLongClickHandler([](Button2 &btn) {
        Serial.println("Button 2 Long");
    });
}

void button_loop()
{
    btn1.loop();
    btn2.loop();
}

void setup()
{

    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS initialisation failed!");
        while (1)
            yield(); // Stay here twiddling thumbs waiting
    }
    Serial.println("\r\nSPIFFS available!");

    // ESP32 will crash if any of the fonts are missing
    bool font_missing = false;
    if (SPIFFS.exists("/NotoSansBold15.vlw") == false)
        font_missing = true;
    if (SPIFFS.exists("/NotoSansBold36.vlw") == false)
        font_missing = true;

    if (font_missing)
    {
        Serial.println("\r\nFont missing in SPIFFS, did you upload it?");
        while (1)
            yield();
    }
    else
        Serial.println("\r\nFonts found OK.");

    Serial.begin(115200);
    Serial.println("Start");

    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    /*
    if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    */

    tft.setRotation(1);
    tft.fillScreen(TFT_RED);

    button_init();

    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars); //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    }
    else
    {
        Serial.println("Default Vref: 1100mV");
    }

    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    /*tft.drawString("LeftButton:", tft.width() / 2, tft.height() / 2 - 16);
    tft.drawString("[WiFi Scan]", tft.width() / 2, tft.height() / 2);
    tft.drawString("RightButton:", tft.width() / 2, tft.height() / 2 + 16);
    tft.drawString("[Voltage Monitor]", tft.width() / 2, tft.height() / 2 + 32);
    tft.drawString("RightButtonLongPress:", tft.width() / 2, tft.height() / 2 + 48);
    tft.drawString("[Deep Sleep]", tft.width() / 2, tft.height() / 2 + 64); */
    tft.setTextDatum(TL_DATUM);
    tft.loadFont(AA_FONT_LARGE); // Must load the font first

    tft.println("I Love"); // println moves cursor down for a new line
    tft.println();
    tft.println("You");
}

void loop()
{

    button_loop();
}