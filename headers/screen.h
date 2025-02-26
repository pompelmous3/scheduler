#ifndef SCREEN_H
#define SCREEN_H
#include "calendar.h"
#include "taskmanager.h"
#include "expensemanager.h"
#include "categorymanager.h"
#include "tool.h"
#include "submodule.h"

#define SC_PADDING_Y 1
#define SC_PADDING_X 2
// TODO: define MIN WIDTH/HEIGHT

// ==== class Screen ====
class Screen {
    int cs_x, cs_y;
    int sc_h, sc_w;
    int fr_h, fr_w; // usable frame height/weight

    int top_y, bottom_y;
    int left_x, right_x;
    int mid_x;
    int cal_end_y;
    int mng_top, tm_bottom; // top,bottom of task/expense_manager

    int mode;
    std::vector<std::shared_ptr<SubModule>> submods;
    int tm_em; // 0: print tm; 1: print em
    bool delegESC;
    bool lockTab;

    std::shared_ptr<Calendar> calendar;
    std::shared_ptr<taskPanel> dateSpecificTasks;
    // std::shared_ptr<Menu> menu;
    std::shared_ptr<taskManager> tm;
    std::shared_ptr<expenseManager> em;
    std::shared_ptr<categoryManager> cm;

    // DBHandler dbh = DBHandler("./scheduler.db");
    std::shared_ptr<DBHandler> dbh;
    std::thread timer_thrd;
    // TODO: current movable range of x and y
public:
    // Screen(int sc_h, int sc_w);
    Screen();
    ~Screen();
    void setMode(int);
    void looping();
    void timer_run();


    void printFrame();
    void printScr();
    void refreshScr();

    int handleOp(int ch);
    void handleRC(int rc);
    void update_dateSpecificTasks();
};

#endif