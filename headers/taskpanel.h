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
    size_t idx; // which task is selected now
    size_t st_idx;
    bool displayIdx; // taskpanel activated, need to highlight selected task
    std::shared_ptr<DBHandler> dbh;
    int h, w;
    size_t tasks_h;
    size_t tasks_w;
    int cur_y;
    int cur_m;
    int cur_d;

public:
    taskPanel(int y, int x, int h, int w, std::string,
        std::shared_ptr<DBHandler>);
    ~taskPanel() override;
    void updateTasks(int y=-1, int m=-1, int d=-1);
    int get_cur_taskid();
    int setDisplayIdx(bool v);
    int handleOp(int ch) override;
    void print() override;
    void setTaskColor(std::string priority, bool selected);
    void resetTaskColor(std::string priority, bool selected);
};

#endif