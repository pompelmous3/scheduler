#ifndef CALENDAR_H
#define CALENDAR_H
#include <string> // need include here so we can use std::string
#include <vector>

// ==== class Month ====
class Month {
    int year;
    int month;
    int start_weekday;
    int total_days;
    int total_weeks;
    int init_x;
    int init_y;
    int **dmap;
    // TODO: destructor
public:
    Month(int yr, int m, int x, int y);
    ~Month();
    int getMonth();
    int getYear();
    int selected_day(int y, int x); // return date in the month on pos(x, y)
    void print();
};

// ==== class Screen ====
class Screen {
    std::vector<Month *> months;
    int d_month_num = 0;
    // TODO: destructor
    // TODO: current movable range of x and y
public:
    Screen();
    ~Screen();
    int getMonthsSize();
    void addMonth(Month *m);
    void refreshScr();
    bool selected(int y, int x);
};

// ==== other functions ====
bool initNcurses();
bool initColors();
void endNcurses();
void loopingMove(Screen &sc); 
int *getYearMonths(int year);
int getTotalWeeks(int td, int sd);
void getTerminalSize(int& height, int&width);
void printYear(int year);
bool isLeapYear(int year);
int getWeekDay(int day, int month, int year);
std::string getMonthStr(int month, int len);


#endif