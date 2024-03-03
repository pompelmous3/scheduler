#ifndef TASKPANEL_H
#define TASKPANEL_H
#include "screenobject.h"
#include "db.h"
#include "tool.h"
#include "log.h"
#include <vector>

class taskPanel : public ScreenObject
{
    std::vector <task_entry> tasks;
    int idx; // which task is selected now
    bool displayIdx;

public:
    taskPanel(int x, int y, int h, int w);
    ~taskPanel();
    void setTasks(std::vector <task_entry> res);
    int setDisplayIdx(bool v);
    int handleOp(int ch);
    void print();

    std::string getTaskStr(std::string state, std::string task);
    void setTaskColor(std::string priority, bool selected);
    void resetTaskColor(std::string priority, bool selected);
};

#endif