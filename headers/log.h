#ifndef Log_H
#define Log_H
#include <string>

class Log {
    std::string path;
    char *getCurTime();
    Log(std::string path);
    // TODO: destructor
public:
    static Log& gI();
    void log(const char *fmt, ...);
};

#define LOG(fmt, ...) Log::gI().log(fmt, ##__VA_ARGS__)
#endif