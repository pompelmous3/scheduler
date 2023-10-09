#ifndef CALENDAR_H
#define CALENDAR_H
#include "db.h"
#include <string> // need include here so we can use std::string
#include <vector>

typedef struct {
    int y;
    int m;
    int d;
} day;

// ==== class Month ====
class Month {
    int year;
    int month;
    int start_weekday;
    int total_days;
    int total_weeks;
    int init_x;
    int init_y;
    int tasks_x; // starting x of tasks
    int tasks_y; // starting y of tasks
    int **dmap;
    
public:
    DBHandler dbh = DBHandler("/var/log/scheduler.db");
    Month(int yr, int m, int x, int y);
    ~Month();
    int getMonth();
    int getYear();
    int selected_day(int y, int x); // return date in the month on pos(x, y)
    void printMonth();
    void printTasks();
};

// ==== class Screen ====
class Screen {
    std::vector<Month *> months;
    int d_month_num = 0;
    // DBHandler dbh = DBHandler("/var/log/scheduler.db");
    int sx;
    int sy;
    
    // TODO: current movable range of x and y
public:
    Screen();
    ~Screen();
    int getMonthsSize();
    void addMonth(Month *m);
    void printScr();
    void refreshScr();
    void move_sx(int x);
    void move_sy(int y);
    day *selected(int y, int x);
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
int getTaskSign(std::string state);
void setTaskColor(std::string priority);
void resetTaskColor(std::string priority);


#endif