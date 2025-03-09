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
    if (static_cast<size_t>(cursorIdx)<vals[valIdx].size()) {
        vals[valIdx].erase(cursorIdx, 1);
    }
}

void inputField::shiftCurs(int i)
{
    cursorIdx += i;
    if (cursorIdx < 0) cursorIdx = 0;
    if (static_cast<size_t>(cursorIdx) > vals[valIdx].size()) cursorIdx = vals[valIdx].size();
}



inputField::inputField(int y_, int x_, std::string n, int mw, int mh)
    : displayField(y_,x_,n,"",mw,mh)
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
        else if (ch==KEY_UP && (cursorIdx>=max_w)) {
            cursorIdx-=max_w;
        } else if (ch==KEY_DOWN && (static_cast<size_t>(cursorIdx+max_w)<=vals[valIdx].size())){
            cursorIdx+=max_w;
        }
    } else if (ch==KEY_END) shiftCurs(vals[valIdx].size());
    else if (ch==KEY_HOME) shiftCurs(-vals[valIdx].size());
    else if (isCtrlD(ch)) this->clear();
    else insCh(ch);

    return res;
}

int inputField::setVal(std::string v)
{
    vals[valIdx] = v;
    if (static_cast<size_t>(cursorIdx)>vals[valIdx].size()) cursorIdx=static_cast<int>(vals[valIdx].size());
    return 0;
}

std::string inputField::getDV()
{
    std::string val = getVal();
    return (val=="")?dfval:val;
}

void inputField::clear()
{
    vals[valIdx] = "";
    cursorIdx = 0;
}

void inputField::print()
{
    // LOG("[IF::print] called on name=[%s]", name.c_str());
    displayField::print();

    size_t pt_curidx = (cursorIdx>=max_w*max_h)? (max_w*max_h-1) : cursorIdx;
    if (selected) {
        // TODO: handle (y,x) for cursorIdx in diff line
        std::string cursCh;
        if (vals[valIdx].empty() || static_cast<size_t>(cursorIdx) >= vals[valIdx].size())
            cursCh = " ";
        else
            cursCh=vals[valIdx].substr(cursorIdx, 1).c_str();
        // mvprintwColor(y, x+cursorIdx, cursCh.c_str(), 108);
        mvprintwColor(y+pt_curidx/max_w,
                        x+(pt_curidx%max_w), cursCh.c_str(), 108);
    }
}