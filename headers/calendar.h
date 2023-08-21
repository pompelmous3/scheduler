#ifndef CALENDAR_H
#define CALENDAR_H
#include <string> // need include here so we can use std::string

bool initNcurses();
bool initColors();
void endNcurses();
int *getYearMonths(int year);
void getTerminalSize(int& height, int&width);
void printYear(int year);
bool isLeapYear(int year);
void printSingleMonth(int month, int year);
int getWeekDay(int day, int month, int year);
std::string getMonthStrCenterAlign(int month, int len);
#endif