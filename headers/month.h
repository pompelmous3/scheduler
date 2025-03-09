#ifndef MONTH_H
#define MONTH_H

#include "tool.h"
#include <map>
#include "taskpanel.h"

#define MON_WIDTH 27 // 7*3 + 6*1(d=1)
#define MON_HEIGHT 9 // max week(6) + 3(header)
#define MON_MARGIN 3 // margin between months
#define DATE_CELL_LEN 3 // Ex: Sun, Mon, ...
#define D_DIST 1 // distance between days in same month


// ==== other functions ====
std::vector<int> getYearMonths(int year);
int getTotalWeeks(int td, int sd);
void getTerminalSize(int& height, int&width);
bool isLeapYear(int year);
int getWeekDay(int d_, int m_, int yr_);
std::string getMonthStrPost(int month, int len);
extern const int days_norm[12];

// ==== day ====
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
    std::map <int, std::map<int, std::vector<int>>> dmap;
    // dmap[y_idx][week_day(not actual x position)] = {date, x_idx}
    int cs_day;
    // taskPanel tp;
    // int taskMode;
    std::pair<int, int> idx;
    bool browsed; //
    bool selected; // this month is selected
    
public:
    // DBHandler dbh = DBHandler("./scheduler.db");
    std::shared_ptr<DBHandler> dbh;
    Month(int, int, int, int, std::shared_ptr<DBHandler>);
    ~Month();
    int getMonth();
    int getYear();
    std::vector<int> getDate();
    std::vector<int> getPos();
    void setBrowsed(int b);
    void setSelected(bool s);
    void shiftPos(int v);
    void shiftIdx(int ch);
    int handleOp(int ch);
    void print();
};


#endif