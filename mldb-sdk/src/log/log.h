#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <cstdlib>
#include <string>

enum Level {
    undefined = -1,

    None = 1,
    Info,
    Warn,
    Error,
    Fatal,
    Debug
};

static Level& log_level() {
    static Level log_level = undefined;
    if (log_level != undefined) {
        return log_level;
    }

    char* lv = std::getenv("MLDB_SDK_LOG");
    if (lv == NULL) {
        log_level = None;
        return log_level;
    }
    switch (std::stoi(lv))
    {
    case Info:
        log_level = Info;
        break;
    case Warn:
        log_level = Warn;
        break;
    case Error:
        log_level = Error;
        break;
    case Fatal:
        log_level = Fatal;
        break;
    case Debug:
        log_level = Debug;
        break;
    case None:
    default:
        log_level = None;
        break;
    }
    return log_level;
}

#define log_stream __cout

#define log_impl(v)                             \
    do {                                        \
        Level& lvl = log_level();               \
        if ((v) <= lvl) {                       \
            /* do others stuff */               \
            std::ostream& __cout = std::cout;

#define lendl std::endl;                        \
        }                                       \
    } while (0)

#define dlog(v) log_impl(v) log_stream

#endif