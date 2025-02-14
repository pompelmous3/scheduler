#ifndef TASKPANEL_H
#define TASKPANEL_H
#include "screenobject.h"
#include "db.h"
#include "tool.h"
#include "log.h"
#include "submodule.h"
#include <vector>

#define PFX_SPACE 5 // spaces before the checkbox
#define CKBX_SZ 2 // checkbox + space


// class taskPanel : public ScreenObject
class taskPanel : public SubModule
{
    std::vector<task_entry> tasks;
    int idx; // which task is selected now
    int st_idx;
    bool displayIdx; // taskpanel activated, need to highlight selected task
    DBHandler dbh = DBHandler("./scheduler.db");
    int h;
    int tasks_h;
    int tasks_w;
    int w;
    int cur_y;
    int cur_m;
    int cur_d;

public:
    taskPanel(int y, int x, int h, int w, std::string);
    ~taskPanel() override;
    void updateTasks(int y=-1, int m=-1, int d=-1);
    int get_cur_taskid();
    int setDisplayIdx(bool v);
    int handleOp(int ch);
    void print();
    void setTaskColor(std::string priority, bool selected);
    void resetTaskColor(std::string priority, bool selected);
};

#endif