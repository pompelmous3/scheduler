#include "displayfield.h"
#include "tool.h"
#include "month.h"
#include "return_code.h"

// ######################## displayField ########################
displayField::displayField(int y_, int x_, std::string n, std::string curv, int mw, int mh)
    : y {y_}, x {x_}, max_w(mw), max_h(mh), name {n}, hovered(false), selected(false),
    valIdx(0), vals(0), cursorIdx(0)
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
        // TODO: check if there are stored categories
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
    } else if (n=="enter") {
        vals.push_back("ENTER");
    }

    std::vector<std::string>::iterator it = vals.begin();
    if (!curv.empty()) it=std::find(vals.begin(), vals.end(), curv);
    else it=vals.begin();

    valIdx = std::distance(vals.begin(), it);
}

displayField::~displayField()
{}

// get/set funcs
int displayField::gety() {return y;}
int displayField::getx() {return x;}
std::string displayField::getname() {return name;}
void displayField::setHovered(bool v) {hovered = v;}
int displayField::setSelected(bool v) {
    int res = 0;
    if (name=="enter") return TM_WRITE_TASK;

    selected = v;
    return res;
    // TODO: check desc buffer
}

int displayField::handleOp(int ch) {
    int res = 0;
    if (isEnter(ch)) {
        return MNGR_STOP_IF;
    }

    // only switch vals
    if (ch==KEY_UP) res=switchV(1);
    else if (ch==KEY_DOWN) res=switchV(-1);

    if (name=="year" || name=="month") res=TM_UPDATE_IF_DAY_RANGE;
    
    return res;
}

void displayField::setValRange(size_t newRange)
{
    if (name != "day") return;
    while (vals.size()>newRange) {
        vals.pop_back();
    }
    while (vals.size()<newRange) {
        vals.push_back(std::to_string(vals.size()+1));
    }

    // newRange may make valIdx out of range
    if (valIdx>vals.size()) valIdx=vals.size()-1;
}

int displayField::setVal(std::string v)
{
    int res = 0;

    for (size_t i=0; i<vals.size(); i++) {
        if (vals[i]==v) {
            valIdx = i;
            break;
        }
    }

    if (name=="year" || name=="month") res=TM_UPDATE_IF_DAY_RANGE;
    return res;
}

void displayField::updateVals(std::vector<std::string> new_vals)
{
    vals = new_vals;
    valIdx %= vals.size();
}

std::string displayField::getVal()
{
    // LOG("[IF::getVal] name=[%s], vals.size()=[%d], valIdx=[%d]",
        // name.c_str(), vals.size(), valIdx);
    return vals[valIdx];
}
std::string displayField::getDV(){
    return getVal();
}
void displayField::print() {
    // LOG("[DF::print] called on name=[%s]", name.c_str());
    // LOG("[DF::print] selected=[%d],hovered=[%d]", selected, hovered);
    std::string ln = selected?getVal():getDV();

    // set color on
    if (selected) {
        attron(COLOR_PAIR(100));
    } else if (hovered) {
        attron(COLOR_PAIR(107));
    } else {
        attron(COLOR_PAIR(10));
    }


    if (max_w!=-1 && max_h!=-1) {
        // LOG("[DF::print] %s: check range, max_w/h=[%d,%d]", name.c_str(), max_w,max_h);
        // check printing range
        size_t mxprg = max_w*max_h; // max print range
        int st_idx = 0; // start printing index
        if (static_cast<size_t>(cursorIdx)>=mxprg) st_idx=static_cast<size_t>(cursorIdx)-mxprg+1;

        for (int i=0; i<max_h; i++) {
            if (i*max_w>=(int)ln.size()) break;
            mvprintw(y+i, x, "%s", ln.substr(st_idx+i*max_w, max_w).c_str());
        }
    } else {
        mvprintw(y, x, "%s", ln.c_str());
        // if (selected) {
        //     // LOG
        //     mvprintwColor(y, x, ln.c_str(), 100);
        // } else if (hovered) {
        //     mvprintwColor(y, x, ln.c_str(), 107);
        // } else {
        //     mvprintwColor(y, x, ln.c_str(), 10);
        // }
    }


    // set color off
    if (selected) {
        attroff(COLOR_PAIR(100));
    } else if (hovered) {
        attroff(COLOR_PAIR(107));
    } else {
        attroff(COLOR_PAIR(10));
    }
}


// ######################## private functions ########################
int displayField::switchV(int i) {
    // LOG("[inputField::switchV] valIdx before: %d", valIdx);
    if (i>0) valIdx = (valIdx+i) % vals.size();
    else valIdx = (valIdx+i+vals.size()) % vals.size();
    // LOG("[inputField::switchV] valIdx after: %d", valIdx);

    if (name=="type") return (i>0)?TM_TYPE_INC:TM_TYPE_DEC;
    return 0;
}


