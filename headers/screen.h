#ifndef SCREEN_H
#define SCREEN_H
#include "calendar.h"
#include "addtaskpanel.h"
#include "tool.h"

// ==== class Screen ====
class Screen {
    std::vector<std::shared_ptr<Month>> months;
    int mon_idx;
    int d_month_num;
    // DBHandler dbh = DBHandler("/var/log/scheduler.db");
    int cs_x, cs_y;
    int sc_h, sc_w;
    int menuMode;
    int atpMode;
    int monthMode;
    std::shared_ptr<Menu> menu;
    std::shared_ptr<addTaskPanel> atp;
    // Menu *menu;
    // addTaskPanel* atp;
    
    // TODO: current movable range of x and y
public:
    // Screen(int sc_h, int sc_w);
    Screen();
    ~Screen();
    void looping();

    int getMonthsSize();
    void addMonth(std::shared_ptr<Month>);
    void printScr();
    void refreshScr();
    void move_cs(int x, int y);
    void shiftMonth(int v);
    void toggleMenuMode();
    void toggleAtpMode();
    void toggleMonthMode();
    void handleArrow(int ch);
    void handleEsc();
    void handleRC(int rc);
    int handleEnter();
    void handleBS();
    int isMenuMode();
    int isAtpMode();
    bool isMonthMode();
    void passOp(int ch);
    // TODO: send to ScreenObject
    // void doMenuOption();
};

#endif