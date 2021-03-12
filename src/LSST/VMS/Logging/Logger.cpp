/*
 * Logger.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: ccontaxis
 */

#include <Logger.h>

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\x1B[37m"

#define BUFFER_SIZE 1024

namespace LSST {
namespace VMS {

void Logger::SetLevel(Levels::Type level) { LOGGER_LEVEL = level; }

void Logger::Trace(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Trace) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_WHITE, "TRACE", buffer);
    va_end(args);
}

void Logger::Debug(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Debug) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_CYAN, "DEBUG", buffer);
    va_end(args);
}

void Logger::Info(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Info) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_GREEN, "INFO", buffer);
    va_end(args);
}

void Logger::Warn(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Warn) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_YELLOW, "WARN", buffer);
    va_end(args);
}

void Logger::Error(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Error) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_RED, "ERROR", buffer);
    va_end(args);
}

void Logger::Fatal(const char *format, ...) {
    if (LOGGER_LEVEL > Levels::Fatal) {
        return;
    }
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    Logger::log(COLOR_MAGENTA, "FATAL", buffer);
    va_end(args);
}

void Logger::log(const char *color, const char *level, const char *message) {
    std::time_t t = time(0);
    struct tm *now = localtime(&t);
    printf("%s%d-%02d-%02dT%02d:%02d:%02d\t%s\t%s%s \n", color, (now->tm_year + 1900), (now->tm_mon + 1),
           now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, level, message, COLOR_NORMAL);
}

} /* namespace VMS */
} /* namespace LSST */
