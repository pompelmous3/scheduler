#ifndef SCREEN_H
#define SCREEN_H
#include "calendar.h"
#include "addtaskpanel.h"

// ==== class Screen ====
class Screen {
    std::vector<Month *> months;
    int d_month_num = 0;
    // DBHandler dbh = DBHandler("/var/log/scheduler.db");
    int cs_x, cs_y;
    int sc_h, sc_w;
    int menuMode = 0;
    int atpMode = 0;
    Menu menu;
    addTaskPanel atp;
    
    // TODO: current movable range of x and y
public:
    Screen(int sc_h, int sc_w);
    ~Screen();
    int getMonthsSize();
    void addMonth(Month *m);
    void printScr();
    void refreshScr();
    void move_cs(int x, int y);
    day *selected(int y, int x);
    void toggleMenuMode();
    void toggleAtpMode();
    void handleEsc();
    void handleEnter();
    int isMenuMode();
    int isAtpMode();
    int passedOp(int ch);
    // TODO: send to ScreenObject
    // void doMenuOption();
};

#endif