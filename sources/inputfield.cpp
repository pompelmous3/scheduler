#include "inputfield.h"
#include "tool.h"
#include "month.h"
#include "return_code.h"

// ######################## inputField ########################
inputField::inputField(int y, int x, std::string n, std::string curv)
    : y {y}, x {x}, name {n}, hovered(false), selected(false),
    valIdx(0), vals(0), dfval(""), acptTyping(false), cursorIdx(0)
{

    if (strcmp(n.c_str(), "year") == 0) {
        for (int i=std::stoi(curv)-10; i<=std::stoi(curv)+200; i++) {
            vals.push_back(toStr(i,4));
        }
    } else if (strcmp(n.c_str(), "month")==0) {
        for (int i=1; i<=12; i++) vals.push_back(toStr(i,2));
    } else if (strcmp(n.c_str(), "day")==0) {
        // the first 2 chars are "total days" of this month
        // the 3,4th chars are current day
        for (int i=1; i<=std::stoi(curv.substr(0,2)); i++) {
            vals.push_back(toStr(i,2));
        }
        curv.erase(0, 2);
    } else if (strcmp(n.c_str(), "hour")==0) {
        for (int i=0; i<=23; i++) vals.push_back(toStr(i,2));
    } else if (strcmp(n.c_str(), "min")==0) {
        for (int i=0; i<=45; i+=15) vals.push_back(toStr(i,2));
    } else if (strcmp(n.c_str(), "priority")==0) {
        vals.push_back("Don't care");
        vals.push_back("Later");
        vals.push_back("NOW!");
    } else if (strcmp(n.c_str(), "category")==0) {
        // TODO: get stored categories
        vals.push_back("None");
        vals.push_back("Personal");
        vals.push_back("Work");
        vals.push_back("Leetcode");
        vals.push_back("Workout");
    } else if (strcmp(n.c_str(), "addCategory")==0) {
        dfval = "<new category>";
        acptTyping = true;
    } else if (strcmp(n.c_str(), "desc")==0) {
        dfval = "<type description>";
        vals.push_back(dfval);
        acptTyping = true;
    } else if (strcmp(n.c_str(), "enter")==0) {
        vals.push_back("ENTER");
    }

    std::vector<std::string>::iterator it = vals.begin();
    if (!curv.empty()) it=std::find(vals.begin(), vals.end(), curv);
    else it=vals.begin();

    valIdx = std::distance(vals.begin(), it);
}

inputField::~inputField()
{}

// get/set funcs
int inputField::gety() {return y;}
int inputField::getx() {return x;}
std::string inputField::getname() {return name;}
void inputField::setHovered(bool v) {hovered = v;}
void inputField::setSelected(bool v) {
    selected = v;
    // TODO: check desc buffer
}

int inputField::handleOp(int ch) {
    int res = 0;
    if (isEnter(ch)) {
        return TM_STOP_IF;
    }

    // TODO: for user to add category
    // if (name=="desc" || (name=="category" && valIdx==vals.size()-1)) {
    if (name=="desc") {
        return handleDescOp(ch);
    } else {
        // only switch vals
        if (ch==KEY_UP) switchV(1);
        else if (ch==KEY_DOWN) switchV(-1);
    }
    return res;
}

std::string inputField::getVal()
{
    return vals[valIdx];
}
void inputField::print() {
    if (selected) {
        mvprintwColor(y, x, getVal().c_str(), 109);
        if (acptTyping) { // only print cursor when selected
            mvprintwColor(y, x+cursorIdx, " ", 108);
        }
    } else if (hovered) {
        mvprintwColor(y, x, getVal().c_str(), 107);
    } else {
        mvprintwColor(y, x, getVal().c_str(), 9);
    }
}


// ######################## private functions ########################
void inputField::switchV(int i) {
    LOG("[inputField::switchV] valIdx before: %d", valIdx);
    if (i>0) valIdx = (valIdx+i) % vals.size();
    else valIdx = (valIdx+i+vals.size()) % vals.size();
    LOG("[inputField::switchV] valIdx after: %d", valIdx);
}

int inputField::handleDescOp(int ch) {
    int res = 0;

    return res;
}
void inputField::insCh(int ch) {
    vals[valIdx].insert(cursorIdx, 1, ch);
}
void inputField::bsCh()
{
    cursorIdx--;
    vals[valIdx].erase(cursorIdx, 1);
}

void inputField::delCh()
{
    vals[valIdx].erase(cursorIdx, 1);
}

void inputField::shiftCurs(int i)
{
    cursorIdx += i;
    if (cursorIdx < 0) cursorIdx = 0;
    if (cursorIdx > vals[valIdx].size()) cursorIdx = vals[valIdx].size();
}
