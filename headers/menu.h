#ifndef MENU_H
#define MENU_H
#include "screenobject.h"
#include <curses.h>
#include <string>
#include <vector>


class Menu : public ScreenObject
{
private:
    std::vector<std::string> options;
    int sl_idx; // selected index for options

public:
    Menu(int sc_h, int sc_w);
    ~Menu();
    void moveOption(int y);
    void print();
    void printSelected();
    int getOptionIdx();
};

#endif