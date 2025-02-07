#include "inputfield.h"
#include "tool.h"
#include "month.h"
#include "return_code.h"

// ######################## inputField ########################
inputField::inputField(int y, int x, std::string n, std::string curv)
    : y {y}, x {x}, name {n}, hovered(false), selected(false),
    valIdx(0), vals(0), dfval(""), acptTyping(false), cursorIdx(0)
{

    if (n=="type") {
        vals.push_back("Date-Specific");
        vals.push_back("General (No Date)");
        vals.push_back("Routine Task");
    } else if (n=="year") {
        for (int i=std::stoi(curv)-10; i<=std::stoi(curv)+200; i++) {
            vals.push_back(toStr(i,4));
        }
    } else if (n=="month") {
        for (int i=1; i<=12; i++) vals.push_back(toStr(i,2));
    } else if (n=="day") {
        // the first 2 chars are "total days" of this month
        // the 3,4th chars are current day
        for (int i=1; i<=std::stoi(curv.substr(0,2)); i++) {
            vals.push_back(toStr(i,2));
        }
        curv.erase(0, 2);
    } else if (n=="hour") {
        for (int i=0; i<=23; i++) vals.push_back(toStr(i,2));
    } else if (n=="min") {
        for (int i=0; i<=45; i+=15) vals.push_back(toStr(i,2));
    } else if (n=="repeat") {
        vals.push_back("Mon");
        vals.push_back("Tue");
        vals.push_back("Wed");
        vals.push_back("Thu");
        vals.push_back("Fri");
        vals.push_back("Sat");
        vals.push_back("Sun");
    } else if (n=="category") {
        // TODO: get stored categories
        vals.push_back("Personal");
        vals.push_back("Work");
        vals.push_back("Workout");
    } else if (n=="priority") {
        vals.push_back("Don't care");
        vals.push_back("Soon");
        vals.push_back("NOW!");
    } else if (n=="state") {
        vals.push_back("TODO");
        vals.push_back("DONE");
    } else if (n=="desc") {
        dfval = "<type description>";
        vals.push_back("");
        acptTyping = true;
    } else if (n=="enter") {
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
int inputField::setSelected(bool v) {
    int res = 0;
    if (name=="enter") return TM_ENTER_PRESS;

    selected = v;
    return res;
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
        if (ch==KEY_UP) res=switchV(1);
        else if (ch==KEY_DOWN) res=switchV(-1);
    }
    return res;
}

std::string inputField::getVal()
{
    return vals[valIdx];
}
std::string inputField::getDV(){
    std::string val = getVal();
    if (name=="desc")
        return ((val=="")?dfval:val);
    return val;
}
void inputField::print() {
    if (selected) {
        mvprintwColor(y, x, getVal().c_str(), 109);
        if (acptTyping) { // only print cursor when selected
            // TODO: handle (y,x) for cursorIdx in diff line
            std::string cursCh;
            if (vals[valIdx].empty() || cursorIdx >= vals[valIdx].size())
                cursCh = " ";
            else
                cursCh=vals[valIdx].substr(cursorIdx, 1).c_str();
            mvprintwColor(y, x+cursorIdx, cursCh.c_str(), 108);
        }
    } else if (hovered) {
        mvprintwColor(y, x, getDV().c_str(), 107);
    } else {
        mvprintwColor(y, x, getDV().c_str(), 9);
    }
}


// ######################## private functions ########################
int inputField::switchV(int i) {
    // LOG("[inputField::switchV] valIdx before: %d", valIdx);
    if (i>0) valIdx = (valIdx+i) % vals.size();
    else valIdx = (valIdx+i+vals.size()) % vals.size();
    // LOG("[inputField::switchV] valIdx after: %d", valIdx);

    if (name=="type") return (i>0)?TM_TYPE_INC:TM_TYPE_DEC;
    return 0;
}

int inputField::handleDescOp(int ch) {
    int res = 0;
    LOG("[IF::handleDescOp] ch=[%d]", ch);
    if (ch==KEY_BACKSPACE) bsCh();
    else if (ch==KEY_DL) delch();
    else if (isArrow(ch)) {
        if (ch==KEY_LEFT) shiftCurs(-1);
        else if (ch==KEY_RIGHT) shiftCurs(1);
    } else if (ch==KEY_END) shiftCurs(vals[valIdx].size());
    else if (ch==KEY_HOME) shiftCurs(-vals[valIdx].size());
    else insCh(ch);

    return res;
}
void inputField::insCh(int ch) {
    vals[valIdx].insert(cursorIdx, 1, ch);
    shiftCurs(1);
}
void inputField::bsCh()
{
    // cursorIdx--;
    if(cursorIdx>0) {
        shiftCurs(-1);
        vals[valIdx].erase(cursorIdx, 1);
    }
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
