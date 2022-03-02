#include <Arduino.h>
#include <Wire.h>
#include "ft6336.h"

enum Ft6336_register {
    FT6336G_REGISTER_DEV_MODE,
    FT6336G_REGISTER_GEST_ID,
    FT6336G_REGISTER_TD_STATUS,
    FT6336G_REGISTER_P1_XH,
    FT6336G_REGISTER_P1_XL,
    FT6336G_REGISTER_P1_YH,
    FT6336G_REGISTER_P1_YL,
    FT6336G_REGISTER_P1_WEIGHT,
    FT6336G_REGISTER_P1_MISC,
    FT6336G_REGISTER_P2_XH,
    FT6336G_REGISTER_P2_XL,
    FT6336G_REGISTER_P2_YH,
    FT6336G_REGISTER_P2_YL,
    FT6336G_REGISTER_P2_WEIGHT,
    FT6336G_REGISTER_P2_MISC,
    FT6336G_REGISTER_TH_GROUP = 0x80,
    FT6336G_REGISTER_TH_DIFF = 0x85,
    FT6336G_REGISTER_CTRL,
    FT6336G_REGISTER_TIMEENTERMONITOR,
    FT6336G_REGISTER_PERIODACTIVE,
    FT6336G_REGISTER_PERIODMONITOR,
    FT6336G_REGISTER_RADIAN_VALUE = 0x91,
    FT6336G_REGISTER_OFFSET_LEFT_RIGHT,
    FT6336G_REGISTER_OFFSET_UP_DOWN,
    FT6336G_REGISTER_DISTANCE_LEFT_RIGHT,
    FT6336G_REGISTER_DISTANCE_UP_DOWN,
    FT6336G_REGISTER_DISTANCE_ZOOM,
    FT6336G_REGISTER_LIB_VER_H = 0xA1,
    FT6336G_REGISTER_LIB_VER_L,
    FT6336G_REGISTER_CIPHER,
    FT6336G_REGISTER_G_MODE,
    FT6336G_REGISTER_PWR_MODE,
    FT6336G_REGISTER_FIRMID,
    FT6336G_REGISTER_FOCALTECH_ID,
    FT6336G_REGISTER_RELEASE_CODE_ID = 0xAF,
    FT6336G_REGISTER_STATE = 0xBC
};

ft6336::ft6336()
{

}

ft6336::~ft6336()
{

}

bool
ft6336::init(TwoWire *wire, uint8_t int_pin)
{
    m_wire = wire;
    m_int_pin = int_pin;

    pinMode(m_int_pin, INPUT_PULLUP);

    write_register(FT6336G_REGISTER_TH_GROUP, 15);
    write_register(FT6336G_REGISTER_G_MODE, 0);
}

bool
ft6336::event_occurred()
{
    return !digitalRead(m_int_pin);
}

ft6336_event
ft6336::read_event()
{
    ft6336_event event;
    uint8_t data[6];

    read_registers(FT6336G_REGISTER_P1_XH, data, 6);

    event.type = data[0] >> 6;
    event.x = ((data[0] & 0xf) << 8) | data[1];
    event.y = (data[2] << 8) | data[3];

    return event;
}

uint32_t
ft6336::read_registers(uint8_t reg, uint8_t *buffer, uint32_t buffer_size)
{
    uint32_t read_count = 0;

    m_wire->beginTransmission(m_device_address);
    m_wire->write(reg);
    m_wire->endTransmission();

    m_wire->requestFrom(m_device_address, 6);
    while (m_wire->available()) {
        if (read_count >= buffer_size) {
            break;
        }

        buffer[read_count++] = m_wire->read();
    }

    return read_count;
}

bool
ft6336::write_register(uint8_t reg, uint8_t data)
{
    m_wire->beginTransmission(m_device_address);
    m_wire->write(reg);
    m_wire->write(data);
    m_wire->endTransmission();
}
