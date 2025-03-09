#include "menu.h"
#include "tool.h"
#include "log.h"

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

void Menu::moveOption(int y_)
{
    sl_idx = (sl_idx + y_) % options.size();
}

void Menu::handleOp(int ch)
{
    LOG("[Menu][handleOp] ch=[%d]", ch);
    if (ch == KEY_UP) {
        moveOption(-1);
    } else if (ch == KEY_DOWN) {
        moveOption(1);
    }
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
    mvprintw(y, x, "%s", ln.c_str());
    attroff(COLOR_PAIR(5));
    x = init_x;
    y = init_y;
}

int Menu::getOptionIdx()
{
    return sl_idx;
}