#ifndef TASKPANEL_H
#define TASKPANEL_H
#include "screenobject.h"
#include "db.h"
#include "tool.h"
#include "log.h"
#include "submodule.h"
#include <vector>

// class taskPanel : public ScreenObject
class taskPanel : public SubModule
{
    std::vector <task_entry> tasks;
    int idx; // which task is selected now
    bool displayIdx; // taskpanel activated, need to highlight selected task
    DBHandler dbh = DBHandler("./scheduler.db");
    int init_x, init_y;
    int cur_y;
    int cur_m;
    int cur_d;

public:
    taskPanel(int y, int x, int h, int w);
    ~taskPanel();
    void updateTasks(int y, int m, int d);
    int setDisplayIdx(bool v);
    int handleOp(int ch);
    void print();

    std::string getTaskStr(std::string state, std::string task);
    void setTaskColor(std::string priority, bool selected);
    void resetTaskColor(std::string priority, bool selected);
};

#endif