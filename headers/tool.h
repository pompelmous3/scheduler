#ifndef TOOL_H
#define TOOL_H
#include <string>
#include "curses.h"
#include "log.h"
#include "return_code.h"
#include <ctime>
#include <chrono>
#include <optional>
#include <map>
#include <memory>


#define KEY_M_ESC 10027


bool initNcurses();
bool initColors();
void mvprintwColor(int y, int x, const char* str, int color);
void endNcurses();

// time related


// string manipulation related
std::tm* curTimeCompnt();
std::string centerText(const std::string& text, int length);
std::string specialWrapCenterText(const std::string& text, int length, char chr);
std::string ljust(const std::string& text, int length, char ch);
std::string rjust(const std::string& text, int length, char ch);
std::string specialLJust(const std::string& text, int length, char ch);
#endif