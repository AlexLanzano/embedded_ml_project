#include <libraries/error.h>
#include <libraries/queue.h>
#include <kernel/task/task_manager.h>
#include <kernel/tasks/input_task.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <mcu/stm32wb55xx/i2c.h>
#include <drivers/input/ft6336g.h>
#include <common/input_event.h>

#define DEBOUNCE_THRESHOLD 10

static bool g_input_task_running = false;
static task_handle_t g_input_task_handle;
static uint32_t g_input_task_stack[500] __attribute__((aligned(4)));
static input_event_t g_input_queue_buffer[10] = {0};
static queue_handle_t g_input_queue;
static gpio_handle_t g_int_pin_handle;
static gpio_handle_t g_button_pin_handle;
static ft6336g_handle_t g_ft6336g_handle;
static uint8_t g_button_state = 1;

static error_t input_task_init_touchscreen()
{
    error_t error;
    i2c_handle_t i2c_handle;

    // Initialize touchscreen
    error = i2c_device_init((i2c_device_configuration_t)
                            {.i2c = I2C1,
                             .address_mode = I2C_ADDRESS_MODE_7BIT,
                             .address = 0x38},
                            &i2c_handle);
    if (error) {
        return error;
    }

    error = gpio_init((gpio_configuration_t)
                      {.port = GPIOA,
                       .pin = 0,
                       .mode = GPIO_MODE_INPUT,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_HIGH,
                       .pull_resistor = GPIO_PULL_RESISTOR_UP},
                      &g_int_pin_handle);
    if (error) {
        return error;
    }

    error = ft6336g_init((ft6336g_configuration_t)
                         {.i2c_handle = i2c_handle,
                          .threshold = 40,
                          .interrupt_mode = FT6336G_INTERRUPT_MODE_POLLING},
                         &g_ft6336g_handle);
    if (error) {
        return error;
    }

    return SUCCESS;
}

static error_t input_task_init_button()
{
    return gpio_init((gpio_configuration_t)
                      {.port = GPIOA,
                       .pin = 1,
                       .mode = GPIO_MODE_INPUT,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_HIGH,
                       .pull_resistor = GPIO_PULL_RESISTOR_UP},
                      &g_button_pin_handle);
}

static void input_task_query_touch_event(bool *display_touched, uint32_t *count)
{
    error_t error;
    uint8_t touch_not_detected = 1;
    input_event_t event = {0};
    uint16_t x, y;
    ft6336g_event_flag_t event_flag;

    gpio_read(g_int_pin_handle, &touch_not_detected);
    if (touch_not_detected) {
        if (*display_touched) {
            if (*count > 1000) {
                event.valid = true;
                event.type = EVENT_TYPE_TOUCH;
                event.touch_event.type = 1;
                event.touch_event.x = x;
                event.touch_event.y = y;
                *count = 0;
                *display_touched = false;
                queue_push(g_input_queue, (void *)&event);
            }
            (*count)++;
        }
        return;
    }

    error = ft6336g_read(g_ft6336g_handle, &x, &y, &event_flag);
    if (error) {
        return;
    }

    if (x > 240 || y > 240) {
        return;
    }

    event.valid = true;
    event.type = EVENT_TYPE_TOUCH;
    event.touch_event.type = event_flag;
    event.touch_event.x = x;
    event.touch_event.y = y;

    if (event.type != 1) {
        event.touch_event.type = TOUCH_EVENT_TYPE_PRESSED;
        *display_touched = true;
    } else {
        *display_touched = false;
    }

    *count = 0;

    queue_push(g_input_queue, (void *)&event);
}

static void input_task_query_button_event(uint8_t *prev_button_state, uint32_t *count)
{
    uint8_t button_state;

    gpio_read(g_button_pin_handle, &button_state);

    if (button_state != *prev_button_state && button_state != g_button_state) {
        *count = 0;

    } else if (button_state == *prev_button_state && button_state != g_button_state) {
        (*count)++;
    }

    if (*count >= DEBOUNCE_THRESHOLD) {
        input_event_t event = {0};
        event.valid = true;
        event.type = EVENT_TYPE_BUTTON;
        event.button_event.type = button_state;
        queue_push(g_input_queue, &event);
        g_button_state = button_state;
        *count = 0;
    }

    *prev_button_state = button_state;
}

error_t input_task_init()
{
    error_t error;

    error = queue_init(10, sizeof(input_event_t), g_input_queue_buffer, &g_input_queue);
    if (error) {
        return error;
    }

    error = input_task_init_touchscreen();
    if (error) {
        return error;
    }

    error = input_task_init_button();
    if (error) {
        return error;
    }

    error = task_manager_init_task(input_task_entry,
                                   1,
                                   500,
                                   g_input_task_stack,
                                   &g_input_task_handle);
    if (error) {
        return error;
    }

    g_input_task_running = true;
    return SUCCESS;
}

error_t input_task_get_event(input_event_t *event)
{
    if (!event) {
        return ERROR_INVALID;
    }

    if (queue_length(g_input_queue) == 0) {
        event->valid = 0;
        return SUCCESS;
    }

    return queue_pop(g_input_queue, (void *)event);
}

void input_task_entry()
{
    bool display_touched = false;
    uint8_t button_state = 1;
    uint32_t touch_count = 0;
    uint32_t button_count = 0;

    while (g_input_task_running) {
        input_task_query_touch_event(&display_touched, &touch_count);
        input_task_query_button_event(&button_state, &button_count);
    }
}
