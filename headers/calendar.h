#ifndef CALENDAR_H
#define CALENDAR_H
#include "db.h"
#include "menu.h"
#include "tool.h"
#include "taskpanel.h"
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
    int sc_h, sc_w;
    int init_x, init_y; // starting position
    int tasks_x, tasks_y; // starting position of tasks in this month
    std::map <int, std::map<int, std::vector<int>>> dmap;
    // dmap[y_idx][week_day(not actual x position)] = {date, x_idx}
    taskPanel tp;
    int taskMode;
    std::pair<int, int> idx;
    bool browsed;
    int selected;
    
public:
    DBHandler dbh = DBHandler("/var/log/scheduler.db");
    Month(int yr, int m, int x, int y, int sc_h, int sc_w);
    ~Month();
    int getMonth();
    int getYear();
    void setBrowsed(int b);
    void setSelected(int s);
    void shiftIdx(int ch);
    int handleOp(int ch);
    void printMonth();
};

// ==== other functions ====

int *getYearMonths(int year);
int getTotalWeeks(int td, int sd);
void getTerminalSize(int& height, int&width);
void printYear(int year);
bool isLeapYear(int year);
int getWeekDay(int day, int month, int year);
std::string getMonthStrPost(int month, int len);

#endif