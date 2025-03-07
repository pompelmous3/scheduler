#include "screenobject.h"
#include "log.h"
#include <curses.h>

ScreenObject::ScreenObject(int sc_h, int sc_w)
{
    height = sc_h/2;
    width = sc_w/3;
    init_x = (sc_w - width) / 2;
    init_y = (sc_h - height) / 2;
    x = init_x;
    y = init_y;
}

ScreenObject::ScreenObject(int y, int x, int h, int w)
{
    height = h;
    width = w;
    init_x = x;
    init_y = y;
    this->x = x;
    this->y = y;
}

ScreenObject::~ScreenObject()
{
}

void ScreenObject::print()
{
    for (size_t i=0; i<printMap.size(); i++) {
        mvprintw(y++, x, printMap[i].c_str());
    }
    x = init_x;
    y = init_y;
}