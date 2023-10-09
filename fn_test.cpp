#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdarg>

void myprintf(const char *fmt, ...);

int main() {
    myprintf("abec [%s], %d", "aaa", 45);
    return 0;
}

void myprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    std::cout << "fmt= " << fmt;
    va_end(args);
}