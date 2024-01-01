#include "../headers/addtaskpanel.h"
#include "../headers/log.h"
#include <curses.h>
#include<iostream>

inputField::inputField(int y, int x, std::string n)
    : sc_y {y}, sc_x {x}, cursorIdx {0}, name {n},
      strValue{std::string("")}, entering {0}, typing {0},
      forAction {false}
{
    // LOG("[inputField::inputField]");
}

inputField::~inputField()
{}

int inputField::gety()
{
    return sc_y;
}

int inputField::getx()
{
    return sc_x;
}

std::string inputField::getname()
{
    return name;
}

int inputField::getTyping()
{
    return typing;
}

int inputField::getCursorIdx()
{
    return cursorIdx;
}

int inputField::geten()
{
    return entering;
}

bool inputField::getAction()
{
    return forAction;
}

void inputField::toggleen()
{
    entering = (entering + 1) % 2;
    if (entering == 0 && typing == 1) {
        typing = 0;
    }
}

intIF::intIF(int y, int x, std::string n, int val)
    : inputField(y, x, n), value {val}
{
    // TODO: set upper and lower limit
    if (strcmp(n.c_str(), "year") == 0) {
        fixedLen = 4;
        upperBnd = 9999;
        lowerBnd = 0;
    } else if (strcmp(n.c_str(), "month") == 0) {
        fixedLen = 2;
        upperBnd = 12;
        lowerBnd = 1;
    } else if (strcmp(n.c_str(), "day") == 0) {
        fixedLen = 2;
        upperBnd = 31;
        lowerBnd = 1;
    } else if (strcmp(n.c_str(), "hour") == 0) {
        fixedLen = 2;
        upperBnd = 23;
        lowerBnd = 0;
    } else if (strcmp(n.c_str(), "min") == 0) {
        fixedLen = 2;
        upperBnd = 59;
        lowerBnd = 0;
    } else {
        LOG("[intIF::intIF] unknown init name=[%s]", n.c_str());
        fixedLen = 2;
    }
}
intIF::~intIF()
{}

std::string intIF::getv()
{
    std::string numStr = std::to_string(value);
    strValue = rjust(numStr, fixedLen, '0');
    return strValue;
}

void intIF::setv(int ch)
{
    LOG("[intIF::setv] unsupported yet");
}

void intIF::backspacev()
{
    LOG("[intIF::backspacev] unsupported yet");
}

void intIF::deletev()
{
    LOG("[intIF::deletev] unsupported yet");
}

void intIF::shiftCurs(int i)
{
    LOG("[intIF::shiftCurs] unsupported yet");
}

void intIF::switchV(int i)
{
    value += i;
    if (value > upperBnd) {
        value = lowerBnd + (value - upperBnd - 1);
    } else if (value < lowerBnd) {
        value = upperBnd - (lowerBnd - value - 1);
    }
}


strIF::strIF(int y, int x, std::string n)
    : inputField(y, x, n), idx {0}
{
    if (n == "state") {
        opts.push_back(std::string("Todo"));
        opts.push_back(std::string("In Progress"));
        opts.push_back(std::string("Done"));
    } else if (n == "priority") {
        opts.push_back(std::string("Urgent"));
        opts.push_back(std::string("High"));
        opts.push_back(std::string("Normal"));
    } else if (n == "description") {
        opts.push_back(std::string("description"));
    } else if (n == "enter") {
        opts.push_back(std::string("ENTER"));
        forAction = 1;
    }
}
strIF::~strIF()
{}

std::string strIF::getv()
{
    strValue = opts[idx];
    return strValue;
}

void strIF::setv(int ch)
{
    if (typing == 0) {
        cursorIdx = 0;
        typing = 1;
    }
    opts[idx].insert(cursorIdx, 1, ch);
    cursorIdx++;
}

void strIF::backspacev()
{
    if (typing == 1 && cursorIdx >= 1) {
        cursorIdx--;
        opts[idx].erase(cursorIdx, 1);
    }
}

void strIF::deletev()
{
    // LOG("[strIF::deletev] before, cursorIdx=[%d], opts[idx].size()=[%d]",
    //     cursorIdx, opts[idx].size());
    if (typing == 1 && cursorIdx < opts[idx].size()) {
        opts[idx].erase(cursorIdx, 1);
    }
}

void strIF::shiftCurs(int i)
{
    cursorIdx += i;
    if (cursorIdx < 0) {
        cursorIdx = 0;
    }
    if (cursorIdx > opts[idx].size()) {
        cursorIdx = opts[idx].size();
    }

}

void strIF::switchV(int i)
{
    int optSize = static_cast<int>(opts.size());
    idx = idx + i;
    if (idx >= optSize) {
        idx = idx % optSize;
    } else if (idx < 0) {
        idx = (optSize - 1) - ((0-idx)-1);
    }
}

void addTaskPanel::init_inputFields()
{
    std::tm* curT = curTimeCompnt();
    // line 1: date (year, month, day)
    std::vector<std::shared_ptr<inputField>> lineOne;
    std::shared_ptr<intIF> year = std::make_shared<intIF>(init_y+3, init_x+15,
        std::string("year"), curT->tm_year+1900);
    lineOne.push_back(std::move(year));
    std::shared_ptr<intIF> month = std::make_shared<intIF>(init_y+3, init_x+20,
        std::string("month"), curT->tm_mon+1);
    lineOne.push_back(std::move(month));
    std::shared_ptr<intIF> day = std::make_shared<intIF>(init_y+3, init_x+23,
        std::string("day"), curT->tm_mday);
    lineOne.push_back(std::move(day));
    inputFields.push_back(std::move(lineOne));

    // line 2: start_time (hour, min)
    std::vector<std::shared_ptr<inputField>> lineTwo;
    std::shared_ptr<intIF> hour = std::make_shared<intIF>(init_y+4, init_x+15,
        std::string("hour"), curT->tm_hour);
    lineTwo.push_back(std::move(hour));
    std::shared_ptr<intIF> min = std::make_shared<intIF>(init_y+4, init_x+18,
        std::string("min"), curT->tm_min);
    lineTwo.push_back(std::move(min));
    inputFields.push_back(lineTwo);

    // line 3: state (default: TODO)
    std::vector<std::shared_ptr<inputField>> lineThree;
    std::shared_ptr<strIF> state = std::make_shared<strIF>(init_y+5, init_x+15,
        std::string("state"));
    lineThree.push_back(std::move(state));
    inputFields.push_back(lineThree);

    // line 4: priority (default: Normal)
    std::vector<std::shared_ptr<inputField>> lineFour;
    std::shared_ptr<strIF> priority = std::make_shared<strIF>(init_y+6, init_x
        +15, std::string("priority"));
    lineFour.push_back(std::move(priority));
    inputFields.push_back(lineFour);

    // line 5: description
    std::vector<std::shared_ptr<inputField>> lineFive;
    std::shared_ptr<strIF> description = std::make_shared<strIF>(init_y+7,
        init_x+15, std::string("description"));
    lineFive.push_back(std::move(description));
    inputFields.push_back(lineFive);

    // line 7:
    std::vector<std::shared_ptr<inputField>> lineSeven;
    std::shared_ptr<strIF> enter = std::make_shared<strIF>(init_y+9,
        init_x+2+(width-4)/2-2, std::string("enter"));
    lineSeven.push_back(std::move(enter));
    inputFields.push_back(lineSeven);

    curPos = std::make_pair(0, 0);
}

int addTaskPanel::getIFColor(int row, int col)
{
    if (curPos.first == row && curPos.second == col) {
        if (inputFields[curPos.first][curPos.second]->geten()) {
            return 8;
        }
        return 14;
    }
    return 2;
}

std::optional<std::pair<int, int>> addTaskPanel::print_inputFields()
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

void addTaskPanel::addTask()
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

addTaskPanel::addTaskPanel(int sc_h, int sc_w)
    : ScreenObject(sc_h, sc_w), enterMode {0}, typingMode {0}
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
    printMap.push_back(specialWrapCenterText("", width, '#'));
    printMap.push_back(std::string(width, '#'));

    init_inputFields();
}

addTaskPanel::~addTaskPanel()
{
    LOG("[~addTaskPanel] called");
}

void addTaskPanel::handleOp(int ch)
{
    // LOG("[addTaskPanel::handleOp] ch=[%d]", ch);
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
                inputFields[enterPos.first][enterPos.second]->shiftCurs(0-width);
                break;
            case KEY_END:
                inputFields[enterPos.first][enterPos.second]->shiftCurs(width);
                break;
            }
            return;
        }
        if (enterMode) { // one of the inputFields is selected, do switch
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
            return;
        }

        switch(ch) { // moving between inputFields
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
        if (inputFields[curPos.first][curPos.second]->getAction()) {
            addTask();
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
}
std::optional<std::pair<int, int>> addTaskPanel::print()
{
    ScreenObject::print();
    std::optional<std::pair<int, int>> curs = print_inputFields();
    return curs;
}

std::string addTaskPanel::getIFValue(std::string name)
{
    for (int row=0; row<inputFields.size(); row++) {
        for (int col=0; col<inputFields[row].size(); col++) {
            if (inputFields[row][col]->getname() == name) {
                return inputFields[row][col]->getv();
            }
        }
    }
    LOG("[getIFValue] cannot find [%s] in inputFields", name);
    return std::string("");
}