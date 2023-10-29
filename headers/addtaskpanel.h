#ifndef ADDTASKPANEL_H
#define ADDTASKPANEL_H
#include "db.h"
#include <string>
#include <vector>

class addTaskPanel
{
private:
    int init_x, init_y;
    int x, y;
    int height, width;
    std::vector <std::string> info_map;
    DBHandler dbh = DBHandler("/var/log/scheduler.db");
public:
    addTaskPanel(int sc_h, int sc_w);
    ~addTaskPanel();
    void print();
};

#endif