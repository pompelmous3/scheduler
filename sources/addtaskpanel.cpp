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
    printMap.push_back(specialLJust(" Date:            /  /  ", width, '#'));
    printMap.push_back(specialLJust(" start_time:    :  ", width, '#'));
    printMap.push_back(specialLJust(" state:     ", width, '#'));
    printMap.push_back(specialLJust(" priority:     ", width, '#'));
    printMap.push_back(specialLJust(" description:     ", width, '#'));
    printMap.push_back(std::string(width, '#'));

    init_inputFields();
}

void addTaskPanel::init_inputFields()
{
    // line 1: date (year, month, day)
    std::vector<inputField> lineOne;
    inputField year = {
        init_y + 3,
        init_x + 15,
        std::string("1999"),
        0,
    };
    inputField month = {
        init_y + 3,
        init_x + 20,
        std::string("12"),
        0,
    };
    inputField day = {
        init_y + 3,
        init_x + 23,
        std::string("12"),
        0,
    };
    lineOne.push_back(year);
    lineOne.push_back(month);
    lineOne.push_back(day);
    inputFields.push_back(lineOne);

    // line 2: start_time ()
    std::vector<inputField> lineTwo;
    inputField hour = {
        init_y + 4,
        init_x + 15,
        std::string("00"),
        0,
    };
    inputField min = {
        init_y + 4,
        init_x + 18,
        std::string("00"),
        0,
    };
    lineTwo.push_back(hour);
    lineTwo.push_back(min);
    inputFields.push_back(lineTwo);

    // line 3: state (default: TODO)
    std::vector<inputField> lineThree;
    inputField state = {
        init_y + 5,
        init_x + 15,
        std::string("TODO"),
        0,
    };
    lineThree.push_back(state);
    inputFields.push_back(lineThree);

    // line 4: priority (default: Normal)
    std::vector<inputField> lineFour;
    inputField priority = {
        init_y + 6,
        init_x + 15,
        std::string("NormaL"),
        0,
    };
    lineFour.push_back(priority);
    inputFields.push_back(lineFour);

    // line 5: description
    std::vector<inputField> lineFive;
    inputField desc = {
        init_y + 7,
        init_x + 15,
        std::string("description"),
        0,
    };
    lineFive.push_back(desc);
    inputFields.push_back(lineFive);


    curPos = std::make_pair(0, 0);
}

int addTaskPanel::getIFColor(int row, int col)
{
    if (curPos.first == row && curPos.second == col) {
        if (inputFields[curPos.first][curPos.second].selected)
            return 8;
        return 14;
    }
    return 10;
}

void addTaskPanel::print_inputFields()
{
    int color;
    for (int row=0; row<inputFields.size(); row++) {
        for (int col=0; col<inputFields[row].size(); col++) {
            color = getIFColor(row, col);
            mvprintwColor(inputFields[row][col].sc_y,
                inputFields[row][col].sc_x,
                inputFields[row][col].value.c_str(),
                color);
        }
    }
}

addTaskPanel::~addTaskPanel()
{
}

void addTaskPanel::handleOp(int ch)
{
    // Log::gI().log("[addTaskPanel][handleOp] entering=%d", entering);
    if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_RIGHT || ch==KEY_LEFT) {
        if (entering) {
            // TODO: pass to inputFields??
            return;
        }
        switch(ch) {
            case KEY_UP:
                if (curPos.first == 0)
                    curPos.first = inputFields.size()-1;
                else
                    curPos.first = (curPos.first - 1) % inputFields.size();

                if (curPos.second >= inputFields[curPos.first].size()) {
                    curPos.second = 0;
                }
                break;
            case KEY_DOWN:
                curPos.first = (curPos.first + 1) % inputFields.size();
                if (curPos.second >= inputFields[curPos.first].size()) {
                    curPos.second = 0;
                }
                break;
            case KEY_LEFT:
                curPos.second = (curPos.second - 1) % inputFields[curPos.first]
                    .size();
                break;
            case KEY_RIGHT:
                curPos.second = (curPos.second + 1) % inputFields[curPos.first]
                    .size();
                break;
        }
    }

    else if (ch == KEY_ENTER) {
        Log::gI().log("setting ENTER on (%d, %d)", curPos.first, curPos.second);
        inputFields[curPos.first][curPos.second].selected += 1;
        inputFields[curPos.first][curPos.second].selected %= 2;
        entering = (entering + 1) % 2;
    }
}
void addTaskPanel::print()
{
    // Log::gI().log("[print] curPos=(%d, %d)", curPos.first, curPos.second);
    ScreenObject::print();
    print_inputFields();
}