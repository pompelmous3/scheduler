#include "inputfield.h"
#include "return_code.h"
#include "log.h"


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
    if (cursorIdx<vals[valIdx].size()) {
        vals[valIdx].erase(cursorIdx, 1);
    }
}

void inputField::shiftCurs(int i)
{
    cursorIdx += i;
    if (cursorIdx < 0) cursorIdx = 0;
    if (cursorIdx > vals[valIdx].size()) cursorIdx = vals[valIdx].size();
}



inputField::inputField(int y, int x, std::string n)
    : displayField(y,x,n)
{

    if (n=="desc") {
        dfval = "<type description>";
        vals.push_back("");
    } else if (n=="new_cat") {
        dfval = "<category>";
        vals.push_back("");
    }
}

inputField::~inputField()
{}

int inputField::handleOp(int ch)
{
    int res = 0;
    if (isEnter(ch)) {
        return MNGR_STOP_IF;
    }

    LOG("[IF::handleTypingOp] ch=[%d]", ch);
    if (isBS(ch)) bsCh();
    else if (isDEL(ch)) delCh();
    else if (isArrow(ch)) {
        if (ch==KEY_LEFT) shiftCurs(-1);
        else if (ch==KEY_RIGHT) shiftCurs(1);
    } else if (ch==KEY_END) shiftCurs(vals[valIdx].size());
    else if (ch==KEY_HOME) shiftCurs(-vals[valIdx].size());
    else insCh(ch);

    return res;
}

int inputField::setVal(std::string v)
{
    vals[valIdx] = v;
    return 0;
}

std::string inputField::getDV()
{
    std::string val = getVal();
    return (val=="")?dfval:val;
}

void inputField::print()
{
    displayField::print();
    if (selected) {
        // TODO: handle (y,x) for cursorIdx in diff line
        std::string cursCh;
        if (vals[valIdx].empty() || cursorIdx >= vals[valIdx].size())
            cursCh = " ";
        else
            cursCh=vals[valIdx].substr(cursorIdx, 1).c_str();
        mvprintwColor(y, x+cursorIdx, cursCh.c_str(), 108);   
    }
}