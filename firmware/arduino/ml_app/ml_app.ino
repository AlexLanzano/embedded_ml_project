/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ST7789.h>
#include "ft6336.h"
#include "button.h"
#include "model.h"

#define BUTTON_PIN 2
#define INT_PIN 5

static ft6336 touch_screen;
static button button;
static Adafruit_ST7789 tft = Adafruit_ST7789(3, 4, 6);
static Model model;

static ft6336_event prev_touch_event;
static bool prev_touch_event_valid = false;

static void handle_touch_event()
{
    if (!touch_screen.event_occurred()) {
        return;
    }

    ft6336_event event = touch_screen.read_event();
    if (event.type == FT6336_EVENT_TYPE_PRESSED) {
        if (prev_touch_event_valid) {
            tft.drawLine(event.x, event.y, prev_touch_event.x, prev_touch_event.y, ST77XX_RED);
            model.draw_line(event.x, event.y, prev_touch_event.x, prev_touch_event.y);
        } else {
            tft.drawPixel(event.x, event.y, ST77XX_RED);
            model.draw_pixel(event.x, event.y);
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

    float output;
    button_event button_event;

    button_event = button.read_event();
    if (button_event == BUTTON_EVENT_TYPE_PRESSED) {
        output = model.execute();
        if (output == 1) {
            Serial1.println("A");
        } else if (output == 0) {
            Serial1.println("B");
        } else {
            Serial1.println(output);
        }
        prev_touch_event_valid = false;
        model.clear_input();
        tft.fillScreen(ST77XX_WHITE);
    }
}

void setup()
{
    Serial1.begin(115200);
    Serial1.println("Begin");
    Wire.begin();

    tft.init(240, 240);
    tft.setRotation(2);

    button.init(BUTTON_PIN);
    touch_screen.init(&Wire, INT_PIN);

    model.init();
}

void loop()
{
    handle_touch_event();
    handle_button_event();
}
