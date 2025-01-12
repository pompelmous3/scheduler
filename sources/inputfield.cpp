#include "inputfield.h"

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