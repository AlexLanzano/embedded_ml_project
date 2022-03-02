#include <Arduino.h>
#include "button.h"

button::button()
{

}

button::~button()
{

}

bool
button::init(uint8_t pin)
{
    m_pin = pin;
    pinMode(pin, INPUT_PULLUP);
}

bool
button::event_occurred()
{
    if (m_event_waiting) {
        return true;
    }

    uint8_t button_state = digitalRead(m_pin);
    if (button_state != m_current_button_state) {
        m_event_waiting = true;
        m_event = button_state;
        return true;
    }

    return false;
}

button_event
button::read_event()
{
    m_event_waiting = false;
    m_current_button_state = m_event;
    return m_current_button_state;
}
