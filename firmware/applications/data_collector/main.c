#include <libraries/log.h>
#include <libraries/input.h>
#include <libraries/graphics.h>

extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];

__attribute__((section(".text.entry")))
void main()
{
    // Copy data section from flash memory to ram
    uint32_t data_section_size = _edata - _sdata;
    memcpy(_sdata, _sidata, data_section_size*4);

    // Zero out bss
    uint32_t bss_section_size = _ebss - _sbss;
    memset(_sbss, 0, bss_section_size*4);

    error_t error;
    rectangle_t rect = (rectangle_t){.x = 0, .y = 0, .width = 240, .height = 240};
    bool previous_point_valid = false;
    uint32_t prev_x, prev_y;

    graphics_init(240, 240);
    graphics_draw_rectangle(rect, COLOR_WHITE);
    graphics_draw_line(10, 10,10,200, COLOR_RED);

    while (1) {
        while(1);
        input_event_t event = {0};

        error = input_poll(&event);
        if (error || !event.valid) {
            continue;
        }

        if (event.type == EVENT_TYPE_TOUCH) {
           if (event.touch_event.type == TOUCH_EVENT_TYPE_PRESSED) {
               log_info("%u %u", event.touch_event.x, event.touch_event.y);
               if (previous_point_valid) {
                   graphics_draw_line(prev_x, prev_y, event.touch_event.x, event.touch_event.x, COLOR_RED);
               } else {
                   graphics_draw_point(event.touch_event.x, event.touch_event.x, COLOR_RED);
                   previous_point_valid = true;
               }
               prev_x = event.touch_event.x;
               prev_y = event.touch_event.y;

           } else if (event.touch_event.type == TOUCH_EVENT_TYPE_RELEASED) {
               previous_point_valid = false;

           }

        } else if (event.type == EVENT_TYPE_BUTTON) {
            if (event.button_event.type == BUTTON_EVENT_TYPE_PRESSED) {
                log_info("DONE");
                //graphics_draw_rectangle(rect, COLOR_WHITE);
            }
        }
    }
}
