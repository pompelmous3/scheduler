#include "../headers/log.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <cstdarg>

// TODO: check log level setting (in where?)

char *Log::getCurTime()
{
    using namespace std;
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();

    tp -= duration_cast<seconds>(tp); // get fraction

    time_t tt = system_clock::to_time_t(now);
    struct tm *local_t = localtime(&tt);

    char *timestr = (char *)malloc(sizeof(char)*100);
    sprintf(timestr, "[%04u-%02u-%02u %02u:%02u:%02u.%03u]", 
        (*local_t).tm_year+1900, (*local_t).tm_mon+1, (*local_t).tm_mday,
        (*local_t).tm_hour, (*local_t).tm_min, (*local_t).tm_sec,
        static_cast<unsigned>(tp / milliseconds(1)));
    return timestr;
}

Log::Log(std::string p)
{
    path = p;
}

Log& Log::gI() // cannot and no need to specify static here
{
    static Log instance("/var/log/scheduler.log");
    return instance;
}

void Log::log(const char *fmt, ...)
{
    char *curT = getCurTime();
    va_list args;
    va_start(args, fmt);
    char buf[1028];
    vsprintf(buf, fmt, args);
    va_end(args);
    std::ofstream out(path, std::ios_base::app);
    out << curT << " " << buf << "\n";
    out.close();
}