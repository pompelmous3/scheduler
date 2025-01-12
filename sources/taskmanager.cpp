#include "taskmanager.h"
#include "log.h"
#include <curses.h>
#include <iostream>


void taskManager::init_inputFields()
{
    std::tm* curT = curTimeCompnt();
    // line 1: date (year, month, day)
    std::vector<std::shared_ptr<inputField>> lineOne;
    std::shared_ptr<intIF> year = std::make_shared<intIF>(y+2, x+14,
        std::string("year"), curT->tm_year+1900);
    lineOne.push_back(std::move(year));
    std::shared_ptr<intIF> month = std::make_shared<intIF>(y+2, x+19,
        std::string("month"), curT->tm_mon+1);
    lineOne.push_back(std::move(month));
    std::shared_ptr<intIF> day = std::make_shared<intIF>(y+2, x+22,
        std::string("day"), curT->tm_mday);
    lineOne.push_back(std::move(day));
    inputFields.push_back(std::move(lineOne));

    // line 2: start_time (hour, min)
    std::vector<std::shared_ptr<inputField>> lineTwo;
    std::shared_ptr<intIF> hour = std::make_shared<intIF>(y+3, x+14,
        std::string("hour"), curT->tm_hour);
    lineTwo.push_back(std::move(hour));
    std::shared_ptr<intIF> min = std::make_shared<intIF>(y+3, x+17,
        std::string("min"), curT->tm_min);
    lineTwo.push_back(std::move(min));
    inputFields.push_back(lineTwo);

    // line 3: state (default: TODO)
    std::vector<std::shared_ptr<inputField>> lineThree;
    std::shared_ptr<strIF> state = std::make_shared<strIF>(y+4, x+14,
        std::string("state"));
    lineThree.push_back(std::move(state));
    inputFields.push_back(lineThree);

    // line 4: priority (default: Normal)
    std::vector<std::shared_ptr<inputField>> lineFour;
    std::shared_ptr<strIF> priority = std::make_shared<strIF>(y+5, x
        +14, std::string("priority"));
    lineFour.push_back(std::move(priority));
    inputFields.push_back(lineFour);

    // line 5: description
    std::vector<std::shared_ptr<inputField>> lineFive;
    std::shared_ptr<strIF> description = std::make_shared<strIF>(y+6,
        x+14, std::string("description"));
    lineFive.push_back(std::move(description));
    inputFields.push_back(lineFive);

    // line 7:
    std::vector<std::shared_ptr<inputField>> lineSeven;
    std::shared_ptr<strIF> enter = std::make_shared<strIF>(y+8,
        x+2+(w-4)/2-2, std::string("enter"));
    lineSeven.push_back(std::move(enter));
    inputFields.push_back(lineSeven);

    curPos = std::make_pair(0, 0);
}

int taskManager::getIFColor(int row, int col)
{
    if (curPos.first == row && curPos.second == col) {
        if (inputFields[curPos.first][curPos.second]->geten()) {
            return 100;
        }
        return 106;
    }
    return 8;
}

std::optional<std::pair<int, int>> taskManager::print_inputFields()
{
    int color;
    int cur_y = 0;
    int cur_x = 0;
    for (int row=0; row<inputFields.size(); row++) {
        for (int col=0; col<inputFields[row].size(); col++) {
            color = getIFColor(row, col);
            mvprintwColor(inputFields[row][col]->gety(),
                inputFields[row][col]->getx(),
                inputFields[row][col]->getv().c_str(),
                color);
            if (inputFields[row][col]->getTyping() == 1) {
                cur_y = inputFields[row][col]->gety();
                cur_x = inputFields[row][col]->getx()
                    + inputFields[row][col]->getCursorIdx();
            }
        }
    }

    if (cur_y == 0 && cur_x == 0) { // no inputField is typing
        return std::nullopt;
    }
    else {
        return std::make_pair(cur_y, cur_x);
    }
}

void taskManager::addTask()
{
    int year = std::stoi(getIFValue(std::string("year")));
    int month = std::stoi(getIFValue(std::string("month")));
    int day = std::stoi(getIFValue(std::string("day")));
    int hour = std::stoi(getIFValue(std::string("hour")));
    int min = std::stoi(getIFValue(std::string("min")));
    std::string start_time = std::to_string(hour) + std::string(":")
        + std::to_string(min);
    std::string state = getIFValue(std::string("state"));
    std::string priority = getIFValue(std::string("priority"));
    std::string desc = getIFValue(std::string("description"));
    dbh.insertTask(year, month, day, start_time, state, priority, desc);
}

void taskManager::shift_curPos(int ch)
{// be care of comparison of int and size_t
    if (ch == KEY_UP || ch == KEY_DOWN) {
        curPos.first = (ch == KEY_UP) ? curPos.first-1 : curPos.first+1;

        // check curPos.first out of range
        if (curPos.first >= (int)inputFields.size()) {
            curPos.first = curPos.first % (int)inputFields.size();
        } else if (curPos.first < 0) {
            curPos.first = ((int)inputFields.size()-1) - (0-curPos.first-1);
        }

        // check if curPos.second out of range due to changed row
        if (curPos.second >= (int)inputFields[curPos.first].size()) {
            curPos.second = (int)inputFields[curPos.first].size()-1;
        }

    } else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
        curPos.second = (ch == KEY_LEFT) ? curPos.second-1 : curPos.second+1;
        // check curPos.second out of range
        if (curPos.second >= (int)inputFields[curPos.first].size()) {
            curPos.second = curPos.second % (int)inputFields[curPos.first]
                .size();
        } else if (curPos.second < 0) {
            curPos.second = ((int)inputFields[curPos.first].size()-1)
                - (0-curPos.second-1);
        }
    }
}

taskManager::taskManager(int y, int x, int h, int w)
    : h(h),w(w),enterMode {0}, typingMode {0}
{
    this->y = y;
    this->x = x;
    this->title = "Task Manager";
    LOG("[taskManager::taskManager] after set: (%d, %d)", y, x);
    // printMap.push_back(std::string(w, '#'));
    // printMap.push_back(specialWrapCenterText("Add a task", w, '#'));
    // printMap.push_back(std::string(w, '#'));
    // printMap.push_back(specialLJust(" Date:            /  /  ", w, '#'));
    // printMap.push_back(specialLJust(" start_time:    :  ", w, '#'));
    // printMap.push_back(specialLJust(" state:     ", w, '#'));
    // printMap.push_back(specialLJust(" priority:     ", w, '#'));
    // printMap.push_back(specialLJust(" description:     ", w, '#'));
    // printMap.push_back(std::string(w, '#'));
    // printMap.push_back(specialWrapCenterText("", w, '#'));
    // printMap.push_back(std::string(w, '#'));


    printMap.push_back(" Date:            /  /  ");
    printMap.push_back(" start_time:    :  ");
    printMap.push_back(" state:     ");
    printMap.push_back(" priority:     ");
    printMap.push_back(" description:     ");
    // printMap.push_back(specialWrapCenterText("", w, '#'));

    init_inputFields();
    LOG("[taskManager::taskManager] after init_inputFields: (%d, %d)", y, x);
}

taskManager::~taskManager()
{
    LOG("[~taskManager] called");
}

int taskManager::handleOp(int ch)
{
    // LOG("[taskManager::handleOp] ch=[%d]", ch);
    if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_RIGHT || ch==KEY_LEFT
        || ch==KEY_END || ch==KEY_HOME) {
        if (typingMode) {
            switch (ch) {
            case KEY_LEFT:
                inputFields[enterPos.first][enterPos.second]->shiftCurs(-1);
                break;
            case KEY_RIGHT:
                inputFields[enterPos.first][enterPos.second]->shiftCurs(1);
                break;
            case KEY_HOME:
                inputFields[enterPos.first][enterPos.second]->shiftCurs(0-w);
                break;
            case KEY_END:
                inputFields[enterPos.first][enterPos.second]->shiftCurs(w);
                break;
            }
        } else if (enterMode) { // one of the inputFields is selected, do switch
            switch (ch) {
            case KEY_UP:
                inputFields[enterPos.first][enterPos.second]->switchV(1);
                break;
            case KEY_DOWN:
                inputFields[enterPos.first][enterPos.second]->switchV(-1);
                break;
            default:
                break;
            }
        } else {
            shift_curPos(ch);
        }
    }

    else if (ch == KEY_ENTER) {
        if (inputFields[curPos.first][curPos.second]->getAction()) {
            addTask();
            return STOP_SC_ATPMODE;
        } else {
            inputFields[curPos.first][curPos.second]->toggleen();
            enterMode = (enterMode + 1) % 2;
            if (enterMode == 0) {
                typingMode = 0;
            }
            enterPos.first = curPos.first;
            enterPos.second = curPos.second;
        }
    } else if (ch == KEY_BACKSPACE) { // Backspace
        if (enterMode) {
            inputFields[curPos.first][curPos.second]->backspacev();
        }
    } else if (ch == KEY_DC) {
        if (enterMode) {
            inputFields[curPos.first][curPos.second]->deletev();
        }
    } else { // not ARROW/ENTER/BACKSPACE
        if (enterMode) {
            typingMode = 1;
            inputFields[curPos.first][curPos.second]->setv(ch);
        }
    }
    return 0;
}
void taskManager::print()
{
    SubModule::print();
    mvprintw(y, x+1+title.size(), " / Expense Manager");

    LOG("[taskManager::print] init (%d, %d)", y, x);

    int py = y+2;
    for (int i=0; i<printMap.size(); i++) {
        mvprintw(py++, x, printMap[i].c_str());
    }
    std::optional<std::pair<int, int>> curs = print_inputFields();
    // return curs;
}

std::string taskManager::getIFValue(std::string name)
{
    for (int row=0; row<inputFields.size(); row++) {
        for (int col=0; col<inputFields[row].size(); col++) {
            if (inputFields[row][col]->getname() == name) {
                return inputFields[row][col]->getv();
            }
        }
    }
    LOG("[getIFValue] cannot find [%s] in inputFields", name.c_str());
    return std::string("");
}