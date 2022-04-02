#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include <stdbool.h>
#include <stdint.h>

#define EVENT_TYPE_TOUCH 0
#define EVENT_TYPE_BUTTON 1

#define TOUCH_EVENT_TYPE_RELEASED 1
#define TOUCH_EVENT_TYPE_PRESSED 2

#define BUTTON_EVENT_TYPE_PRESSED 0
#define BUTTON_EVENT_TYPE_RELEASED 1

typedef struct input_event {
    bool valid;
    uint32_t type;
    union {
        struct {
            uint32_t type;
            uint32_t x;
            uint32_t y;
        } touch_event;
        struct {
            uint8_t type;
        } button_event;
    };
} input_event_t;

#endif
