#ifndef TOOL_H
#define TOOL_H
#include <string>

std::string centerText(const std::string& text, int length);
std::string specialWrapCenterText(const std::string& text, int length, char chr);
std::string ljust(const std::string& text, int length);
std::string specialLJust(const std::string& text, int length, char ch);
#endif