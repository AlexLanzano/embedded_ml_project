#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

enum button_event_type {
    BUTTON_EVENT_TYPE_PRESSED,
    BUTTON_EVENT_TYPE_RELEASED
};

typedef uint8_t button_event;

class button {
public:
    button();
    ~button();
    bool init(uint8_t pin);
    bool event_occurred();
    button_event read_event();
private:
    uint8_t m_pin;
    uint8_t m_current_button_state;
    bool m_event_waiting = false;
    button_event m_event;

};

#endif
