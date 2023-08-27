#ifndef LOGGING_H
#define LOGGING_H
#include <string>

class Logging {
    std::string path;
    char *getCurTime();
    Logging(std::string path);
    // TODO: destructor
public:
    static Logging& getInstance();
    void log(const char *fmt, ...);
};
#endif