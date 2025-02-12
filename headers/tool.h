#ifndef TOOL_H
#define TOOL_H
#include <string>
#include <vector>
#include "curses.h"
#include "string.h"
#include <ctime>
#include <chrono>
#include <optional>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <thread>

// defs
#define KEY_M_ESC 27
#define KEY_TAB 9

#define MNG_HEIGHT 10

bool initNcurses();
void m_init_color(int, int, int, int);
bool initColors();
void mvprintwColor(int y, int x, const char* str, int color, bool bold=false);
void endNcurses();

// KEYs
bool isArrow(int ch);
bool isEnter(int ch);
bool isESC(int ch);
bool isBS(int ch);
bool isDEL(int ch);
bool isCtrlE(int ch);
// time related


// string manipulation related
std::tm* curTimeCompnt();
std::string centerText(const std::string& text, int length);
std::string specialWrapCenterText(const std::string& text, int length, char chr);
std::string ljust(const std::string& text, int length, char ch);
std::string rjust(const std::string& text, int length, char ch);
std::string specialLJust(const std::string& text, int length, char ch);
std::string toStr(int n, int len);
#endif