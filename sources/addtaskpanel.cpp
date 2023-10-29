#include "../headers/addtaskpanel.h"
#include "../headers/log.h"
#include "../headers/tool.h"
#include <curses.h>

addTaskPanel::addTaskPanel(int sc_h, int sc_w)
{
    height = sc_h/2;
    width = sc_w/3;
    init_x = (sc_w - width) / 2;
    init_y = (sc_h - height) / 2;
    x = init_x;
    y = init_y;

    // initialize info_map
    
    info_map.push_back(std::string(width, '#'));
    info_map.push_back(specialWrapCenterText("Add a task", width, '#'));
    info_map.push_back(std::string(width, '#'));
    info_map.push_back(specialLJust(" Date:     /  /  ", width, '#'));
    info_map.push_back(specialLJust(" start_time:     ", width, '#'));
    info_map.push_back(specialLJust(" state:     ", width, '#'));
    info_map.push_back(specialLJust(" priority:     ", width, '#'));
    info_map.push_back(specialLJust(" description:     ", width, '#'));
    info_map.push_back(std::string(width, '#'));
}

addTaskPanel::~addTaskPanel()
{
}

void addTaskPanel::print()
{
    x = init_x;
    y = init_y;
    for (int i=0; i<info_map.size(); i++) {
        mvprintw(y++, x, info_map[i].c_str());
    }
}