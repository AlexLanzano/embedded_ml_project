#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ST7789.h>
#include "ft6336.h"
#include "button.h"

#define BUTTON_PIN 2
#define INT_PIN 5

static ft6336 touch_screen;
static button button;
static Adafruit_ST7789 tft = Adafruit_ST7789(3, 4, 6);

static ft6336_event prev_touch_event;
static bool prev_touch_event_valid = false;

static void print_touch_event(arduino::UART *serial, ft6336_event event)
{
    serial->print(event.x);
    serial->print(" ");
    serial->println(event.y);
}

static void handle_touch_event()
{
    if (!touch_screen.event_occurred()) {
        return;
    }

    ft6336_event event = touch_screen.read_event();
    if (event.type == FT6336_EVENT_TYPE_PRESSED) {
        print_touch_event(&Serial1, event);
        if (prev_touch_event_valid) {
            tft.drawLine(event.x, event.y, prev_touch_event.x, prev_touch_event.y, ST77XX_RED);
        } else {
            tft.drawPixel(event.x, event.y, ST77XX_RED);
            prev_touch_event_valid = true;
        }
        prev_touch_event = event;
    } else if (event.type == FT6336_EVENT_TYPE_RELEASED) {
        prev_touch_event_valid = false;
    }
}

static void handle_button_event()
{
    if (!button.event_occurred()) {
        return;
    }

    button_event button_event;

    button_event = button.read_event();
    if (button_event == BUTTON_EVENT_TYPE_PRESSED) {
        Serial1.println("Done");
        prev_touch_event_valid = false;
        tft.fillScreen(ST77XX_WHITE);
    }
}

void setup()
{
    Serial1.begin(115200);
    Wire.begin();

    tft.init(240, 240);
    tft.setRotation(2);

    button.init(BUTTON_PIN);
    touch_screen.init(&Wire, INT_PIN);
}

void loop()
{
    handle_touch_event();
    handle_button_event();
}
