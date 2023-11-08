#include "../headers/addtaskpanel.h"
#include "../headers/log.h"
#include "../headers/tool.h"
#include <curses.h>

addTaskPanel::addTaskPanel(int sc_h, int sc_w)
    : ScreenObject(sc_h, sc_w)
{
    printMap.push_back(std::string(width, '#'));
    printMap.push_back(specialWrapCenterText("Add a task", width, '#'));
    printMap.push_back(std::string(width, '#'));
    printMap.push_back(specialLJust(" Date:     /  /  ", width, '#'));
    printMap.push_back(specialLJust(" start_time:     ", width, '#'));
    printMap.push_back(specialLJust(" state:     ", width, '#'));
    printMap.push_back(specialLJust(" priority:     ", width, '#'));
    printMap.push_back(specialLJust(" description:     ", width, '#'));
    printMap.push_back(std::string(width, '#'));

    init_inputFields();

    // inputFields
    // inputField.push_back(std::string("year"));
    // inputFieldPos.push_back
    // inputField.push_back(std::string("month"));
    // inputField.push_back(std::string("day"));
    // inputField.push_back(std::string("state"));
    // inputField.push_back(std::string("priority"));
    // inputField.push_back(std::string("description"));
}

void addTaskPanel::init_inputFields()
{
    // line 1: date (year, month, day)
    std::vector<inputField> lineOne;
    inputField year = {
        init_y + 3,
        init_x + 8,
        std::string("1999"),
    };
    inputField month = {
        init_y + 3,
        init_x + 13,
        std::string("12"),
    };
    inputField day = {
        init_y + 3,
        init_x + 16,
        std::string("12"),
    };
    lineOne.push_back(year);
    lineOne.push_back(month);
    lineOne.push_back(day);
    inputFields.push_back(lineOne);

    // line 2: 
    curPos = std::make_pair(0, 0);
}

void addTaskPanel::print_inputFields()
{
    int color = 10;
    for (int row=0; row<inputFields.size(); row++) {
        for (int col=0; col<inputFields[row].size(); col++) {
            mvprintwColor(inputFields[row][col].sc_y,
                inputFields[row][col].sc_x,
                inputFields[row][col].value.c_str(),
                (curPos.first == row && curPos.second == col) ? 14 : 10);
        }
    }
}

addTaskPanel::~addTaskPanel()
{
}

void addTaskPanel::handleOp(int ch)
{
    Log::gI().log("[addTaskPanel][handleOp] ch=[%d]", ch);
}
void addTaskPanel::print()
{
    ScreenObject::print();
    print_inputFields();
    // x = init_x;
    // y = init_y;
    // for (int i=0; i<printMap.size(); i++) {
    //     mvprintw(y++, x, printMap[i].c_str());
    // }
}