// Log.h — Simple unified logging helpers (info / warn / error).
// Use `Log::info("fmt %d", v);` from any translation unit.
#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace Log {

inline void vprint(FILE* out, const char* level, const char* fmt, va_list ap) {
    char buf[512];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm);
    fprintf(out, "%s [%s] %s\n", timebuf, level, buf);
}

inline void info(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprint(stdout, "INFO", fmt, ap);
    va_end(ap);
}

inline void warn(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprint(stderr, "WARN", fmt, ap);
    va_end(ap);
}

inline void error(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprint(stderr, "ERROR", fmt, ap);
    va_end(ap);
}

} // namespace Log

#endif // LOG_H
