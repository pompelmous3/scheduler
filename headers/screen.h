#ifndef SCREEN_H
#define SCREEN_H
#include "calendar.h"
#include "addtaskpanel.h"
#include "tool.h"
#include "submodule.h"

#define SC_PADDING_Y 1
#define SC_PADDING_X 2
// TODO: define MIN WIDTH/HEIGHT

// ==== class Screen ====
class Screen {
    int cs_x, cs_y;
    int sc_h, sc_w;
    int top_y, bottom_y;
    int left_x, right_x;
    int mid_x;
    int cal_end_y;

    int mode;
    std::vector<std::shared_ptr<SubModule>> submods;
    bool delegESC;

    std::shared_ptr<Calendar> calendar;
    std::shared_ptr<taskPanel> dailytasks;
    // std::shared_ptr<Menu> menu;
    // std::shared_ptr<addTaskPanel> atp;
    
    // TODO: current movable range of x and y
public:
    // Screen(int sc_h, int sc_w);
    Screen();
    ~Screen();
    void looping();


    void printFrame();
    void printScr();
    void refreshScr();

    int handleOp(int ch);
    void handleRC(int rc);
    void update_dailytasks();
};

#endif