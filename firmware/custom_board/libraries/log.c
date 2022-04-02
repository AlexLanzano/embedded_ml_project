#include <common/log_message.h>
#include <libraries/string.h>
#include <libraries/log.h>
#include <libraries/system.h>

static void log_print(const char *format, log_message_t *log_message, va_list ap)
{
    if (!format || !log_message) {
        return;
    }

    string_t message = string_init(log_message->message_data, LOG_MSG_MAX_LENGTH);
    char format_data[LOG_MSG_MAX_LENGTH] = {0};
    string_t format_string = string_init_from_cstring(format_data, format, LOG_MSG_MAX_LENGTH);

    if (message.error || format_string.error) {
        return;
    }

    string_format(&message, format_string, ap);
    log_message->message = message;

    system_call(SYSTEM_CALL_LOG, (void *)log_message);
}

void log(log_level_t log_level, const char *format, ...)
{
    if (!format) {
        return;
    }

    va_list ap;
    log_message_t log_message = {0};
    log_message.level = log_level;

    va_start(ap, format);
    log_print(format, &log_message, ap);
    va_end(ap);
}

void log_error(error_t error, const char *format, ...)
{
    if ( !format) {
        return;
    }

    va_list ap;
    log_message_t log_message;
    log_message.level = LOG_LEVEL_ERROR;
    log_message.error = error;

    va_start(ap, format);
    log_print(format, &log_message, ap);
    va_end(ap);
}

void log_info(const char *format, ...)
{
    if (!format) {
        return;
    }

    va_list ap;
    log_message_t log_message;
    log_message.level = LOG_LEVEL_INFO;

    va_start(ap, format);
    log_print(format, &log_message, ap);
    va_end(ap);
}

void log_debug(const char *format, ...)
{
    if (!format) {
        return;
    }

    va_list ap;
    log_message_t log_message;
    log_message.level = LOG_LEVEL_DEBUG;

    va_start(ap, format);
    log_print(format, &log_message, ap);
    va_end(ap);
}
