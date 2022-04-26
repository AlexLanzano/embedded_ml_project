#ifndef FT6336_H
#define FT6336_H

#include <Arduino.h>
#include <Wire.h>

enum ft6336_event_type {
    FT6336_EVENT_TYPE_PRESSED=2,
    FT6336_EVENT_TYPE_RELEASED=1
};

struct ft6336_event {
    uint8_t type;
    uint8_t x;
    uint8_t y;
};

class ft6336 {
public:
    ft6336();
    ~ft6336();
    bool init(TwoWire *wire, uint8_t int_pin);
    bool event_occurred();
    ft6336_event read_event();
private:
    uint32_t read_registers(uint8_t reg, uint8_t *buffer, uint32_t buffer_size);
    bool write_register(uint8_t reg, uint8_t data);

    TwoWire *m_wire = nullptr;
    uint8_t m_int_pin;
    const uint8_t m_device_address = 0x38;
};

#endif
