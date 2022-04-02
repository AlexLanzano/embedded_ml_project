#include <stdint.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/graphics.h>
#include <libraries/system.h>
#include <libraries/math.h>
#include <common/rectangle.h>
#include <common/render_request.h>

extern uint16_t _framebuffer_start[];
extern uint16_t _framebuffer_end[];
static uint32_t g_display_width;

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

    bounding_rect.x = (x1 < x2) ? x1 : x2;
    bounding_rect.y = (y1 < y2) ? y1 : y2;
    bounding_rect.width = abs(x2 - x1) + 1;
    bounding_rect.height = abs(y2 - y1) + 1;

    return bounding_rect;
}

static error_t graphics_draw_line_low(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;
    int32_t dx, dy, D, yi;
    uint32_t y;

    rect = graphics_bounding_rect_from_line(x1, y1, x2, y2);

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;

    dx = (int32_t)x2 - (int32_t)x1;
    dy = (int32_t)y2 - (int32_t)y1;
    yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    D = (2 * dy) - dx;
    y = y1;

    for (uint32_t x = x1; x < x2; x++) {
        framebuffer[g_display_width * y + x] = color;
        if (D > 0) {
            y += yi;
            D += (2 * (dy - dx));
        } else {
            D = D + 2*dy;
        }
    }

    memcpy(&request.rect, &rect, sizeof(render_request_t));
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}

static error_t graphics_draw_line_high(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;
    int32_t dx, dy, D, xi;
    uint32_t x;

    rect = graphics_bounding_rect_from_line(x1, y1, x2, y2);

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;

    dx = x2 - x1;
    dy = y2 - y1;
    xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    D = (2 * dx) - dy;
    x = x1;

    for (uint32_t y = y1; y < y2; y++) {
        framebuffer[g_display_width * y + x] = color;
        if (D > 0) {
            x += xi;
            D += (2 * (dx - dy));
        } else {
            D = D + 2*dx;
        }
    }

    memcpy(&request.rect, &rect, sizeof(render_request_t));
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}

static error_t graphics_draw_vertical_line(uint32_t x1, uint32_t y1, uint32_t y2, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;

    rect = graphics_bounding_rect_from_line(x1, y1, x1, y2);

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;

    for (uint32_t y = y1; y < y2; y++) {
        framebuffer[g_display_width * y + x1] = color;
    }

    memcpy(&request.rect, &rect, sizeof(render_request_t));
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
}

static error_t graphics_draw_horizontal_line(uint32_t x1, uint32_t y1, uint32_t x2, color16_t color)
{
    error_t error;
    rectangle_t rect;
    color16_t *framebuffer;
    render_request_t request = {0};
    bool render_complete = false;

    rect = graphics_bounding_rect_from_line(x1, y1, x2, y1);

    error = graphics_validate_rect(rect);
    if (error) {
        return error;
    }

    framebuffer = (color16_t *)_framebuffer_start;

    for (uint32_t x = x1; x < x2; x++) {
        framebuffer[g_display_width * y1 + x] = color;
    }

    memcpy(&request.rect, &rect, sizeof(render_request_t));
    request.render_complete = &render_complete;

    system_call(SYSTEM_CALL_RENDER, (void *)&request);
    while (!render_complete);

    return SUCCESS;
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
    if (x1 == x2) {
        return graphics_draw_vertical_line(x1, y1, y2, color);

    } else if (y1 == y2) {
        return graphics_draw_horizontal_line(x1, y1, x2, color);

    } else if (abs(y2 - y1) < abs(x2 - x1)) {
        if (x1 > x2) {
            return graphics_draw_line_low(x2, y2, x1, y1, color);
        } else {
            return graphics_draw_line_low(x1, y1, x2, y2, color);
        }
    } else {
        if (y1 > y2) {
            return graphics_draw_line_high(x2, y2, x1, y1, color);
        } else {
            return graphics_draw_line_high(x1, y1, x2, y2, color);
        }
    }

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
