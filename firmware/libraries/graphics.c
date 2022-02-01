#include <stdint.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/graphics.h>
#include <libraries/system.h>
#include <common/rectangle.h>
#include <common/render_request.h>

extern uint16_t _framebuffer_start[];
extern uint16_t _framebuffer_end[];
static uint32_t g_display_width;
// static uint32_t g_display_height;

static error_t graphics_validate_rect(rectangle_t rect)
{
    return SUCCESS;
}

static error_t graphics_get_framebuffer_address(rectangle_t rect, color16_t **framebuffer)
{
    if (!framebuffer) {
        return ERROR_INVALID;
    }

    *framebuffer = (color16_t *)_framebuffer_start + (g_display_width * rect.y) + rect.x;
    return SUCCESS;
}

static rectangle_t graphics_bounding_rect_from_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    rectangle_t bounding_rect = {0};

    if (x1 > x2) {
        uint32_t temp = x2;
        x2 = x1;
        x1 = temp;
        temp = y2;
        y2 = y1;
        y1 = temp;
    }

    bounding_rect.x = x1;
    bounding_rect.y = (y1 < y2) ? y1 : y2;
    bounding_rect.width = x2 - x1;
    bounding_rect.height = y2 - y1;

    return bounding_rect;
}

error_t graphics_init(uint32_t display_width, uint32_t display_height)
{
    // TODO: Validate display width and height
    g_display_width = display_width;
    g_display_width = display_height;
    return SUCCESS;
}

error_t graphics_draw_rectangle(rectangle_t rect, color16_t color)
{
    error_t error;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    error = graphics_get_framebuffer_address(rect, &framebuffer);
    if (error) {
        return error;
    }

    for (uint32_t i = 0; i < rect.height; i++) {
        memset16(framebuffer, color, rect.width);
        framebuffer += g_display_width;
    }

    request.rect = rect;
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}

error_t graphics_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;
    uint32_t dx, dy;

    rect = graphics_bounding_rect_from_line(x1, y1, x2, y2);

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;

    if (x1 > x2) {
        uint32_t temp = x2;
        x2 = x1;
        x1 = temp;
        temp = y2;
        y2 = y1;
        y1 = temp;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    for (uint32_t x = x1; x < x2; x++) {
        uint32_t y;
        if (dx == 0) {
            y = x;
        } else if (dy == 0) {
            y = y1;
        } else {
            y = y1 + dy * (x - x1) / dx;
        }
        framebuffer[g_display_width * y + x] = color;
    }

    request.rect = rect;
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}

error_t graphics_draw_point(uint32_t x, uint32_t y, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;

    rect = (rectangle_t){.x = x, .y = y, .width = 1, .height = 1};

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;
    framebuffer[g_display_width * y + x] = color;

    request.rect = rect;
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}
