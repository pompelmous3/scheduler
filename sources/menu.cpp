#include "../headers/menu.h"
#include "../headers/tool.h"
#include "../headers/log.h"

Menu::Menu(int sc_h, int sc_w)
    : ScreenObject(sc_h, sc_w)
{
    printMap.push_back(std::string(width, '#'));
    printMap.push_back(specialWrapCenterText("Add a task", width, '#'));
    printMap.push_back(specialWrapCenterText("Exit", width, '#'));
    printMap.push_back(std::string(width, '#'));

    options.push_back(std::string("Add a task"));
    options.push_back(std::string("Exit"));
    sl_idx = 0;
}

Menu::~Menu()
{
}

void Menu::moveOption(int y)
{
    sl_idx = (sl_idx + y) % options.size();
}

void Menu::handleOp(int ch)
{
    Log::gI().log("[Menu][handleOp] ch=[%d]", ch);
}

void Menu::print()
{
    ScreenObject::print();
    printSelected();
}

void Menu::printSelected()
{
    std::string ln = centerText(options[sl_idx], width-2);
    y += (1+sl_idx);
    x += 1;
    attron(COLOR_PAIR(5));
    mvprintw(y, x, ln.c_str());
    attroff(COLOR_PAIR(5));
    x = init_x;
    y = init_y;
}

int Menu::getOptionIdx()
{
    return sl_idx;
}