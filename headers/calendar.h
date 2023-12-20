#ifndef CALENDAR_H
#define CALENDAR_H
#include "db.h"
#include "menu.h"
#include "tool.h"
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
    int init_x, init_y; // starting position
    int tasks_x, tasks_y; // starting position of tasks in this month
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

// ==== other functions ====

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