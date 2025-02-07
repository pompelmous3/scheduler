#include "taskmanager.h"
#include "log.h"
#include "tool.h"
#include "month.h"
#include "return_code.h"
// #include <curses.h>
// #include <iostream>


void taskManager::init_fields()
{
    std::tm* curT = curTimeCompnt();
    int cyr = curT->tm_year+1900;
    std::vector<int >dofm = getYearMonths(cyr);
    int cm = curT->tm_mon+1;
    int cd = curT->tm_mday;
    int ch = curT->tm_hour;
    int cmin = curT->tm_min;
    cmin = ((cmin/15 + 1)*15)%60;
    if (cmin==0) ch+=1;
    if (ch==24) {ch=0; cd+=1;};
    if (cd>dofm[cm-1]) {cd=1; cm+=1;}
    if (cm>12) {cm=1; cyr+=1;}
    

    type = std::make_shared<inputField>(
        y+2,
        x+12,
        std::string("type")
    );

    /*
    Following are all fields for all types. Lines are "copied" into
    fields, so we can still access those fields later.
    */
    // ################ line 1 ################
    std::vector<std::shared_ptr<inputField>> lineOne;
    std::shared_ptr<inputField> year = std::make_shared<inputField>(
        y+3,
        x+12,
        std::string("year"),
        toStr(cyr, 4)
    );
    lineOne.push_back(std::move(year));

    std::shared_ptr<inputField> month = std::make_shared<inputField>(
        y+3,
        x+17,
        std::string("month"),
        toStr(cm, 2)
    );
    lineOne.push_back(std::move(month));

    std::shared_ptr<inputField> day = std::make_shared<inputField>(
        y+3,
        x+20,
        std::string("day"),
        toStr(dofm[cm-1],2)+toStr(cd, 2)
    );
    lineOne.push_back(std::move(day));
    // fields.push_back(std::move(lineOne));

    std::shared_ptr<inputField> hour = std::make_shared<inputField>(
        y+3,
        x+32,
        std::string("hour"),
        toStr(ch, 2)
    );
    lineOne.push_back(std::move(hour));

    std::shared_ptr<inputField> min = std::make_shared<inputField>(
        y+3,
        x+35,
        std::string("min"),
        toStr(cmin, 2)
    );
    lineOne.push_back(std::move(min));
    fields[0].push_back(lineOne);
    std::shared_ptr<inputField> repeat = std::make_shared<inputField>(
        y+3,
        x+54,
        std::string("repeat")
    );
    lineOne.push_back(std::move(repeat));
    fields[2].push_back(lineOne);


    // ################ line 2  ################
    std::vector<std::shared_ptr<inputField>> lineTwo;
    std::shared_ptr<inputField> category = std::make_shared<inputField>(
        y+4,
        x+12,
        std::string("category")
    );
    lineTwo.push_back(std::move(category));
    // TODO: add "new category" field
    fields[0].push_back(lineTwo);
    fields[1].push_back(lineTwo);
    fields[2].push_back(lineTwo);

    // ################ line 3 ################
    std::vector<std::shared_ptr<inputField>> lineThree;
    std::shared_ptr<inputField> priority = std::make_shared<inputField>(
        y+5,
        x+12,
        std::string("priority")
    );
    lineThree.push_back(std::move(priority));
    std::shared_ptr<inputField> state = std::make_shared<inputField>(
        y+5,
        x+32,
        std::string("state")
    );
    lineThree.push_back(std::move(state));
    fields[0].push_back(lineThree);
    fields[1].push_back(lineThree);
    fields[2].push_back(lineThree);

    // ################ line 4 ################
    std::vector<std::shared_ptr<inputField>> lineFour;
    std::shared_ptr<inputField> desc = std::make_shared<inputField>(
        y+6,
        x+12,
        std::string("desc")
    );
    lineFour.push_back(std::move(desc));
    fields[0].push_back(lineFour);
    fields[1].push_back(lineFour);
    fields[2].push_back(lineFour);

    // ################ line 6 ################
    std::vector<std::shared_ptr<inputField>> lineSix;
    std::shared_ptr<inputField> enter = std::make_shared<inputField>(
        y+8,
        x+2+(w-4)/2-2,
        std::string("enter")
    );
    lineSix.push_back(std::move(enter));
    fields[0].push_back(lineSix);
    fields[1].push_back(lineSix);
    fields[2].push_back(lineSix);

    ifx = -1; ify = 0;
    type->setHovered(true);
}

void taskManager::writeTask()
{
    LOG("[TM::writeTask] enter");
    // int year = std::stoi(getIFValue("year"));
    // int month = std::stoi(getIFValue("month"));
    // int day = std::stoi(getIFValue("day"));
    std::string year = getIFValue("year");
    std::string month = getIFValue("month");
    std::string day = getIFValue("day");
    std::string hour = getIFValue("hour");
    std::string min = getIFValue("min");
    std::string start_time = hour+":"+min;
    std::string repeat = getIFValue("repeat");
    std::string cat = getIFValue("category");
    std::string priority = getIFValue("priority");
    std::string state = getIFValue("state");
    std::string desc = getIFValue("desc");


    LOG("[TM::writeTask] Date: %s/%s/%s ", year.c_str(), month.c_str(), day.c_str());
    LOG("[TM::writeTask] start_time=[%s]", start_time.c_str());
    LOG("[TM::writeTask] repeat=[%s]", repeat.c_str());
    LOG("[TM::writeTask] cat=[%s]", cat.c_str());
    LOG("[TM::writeTask] priority=[%s]", priority.c_str());
    LOG("[TM::writeTask] state=[%s]", state.c_str());
    LOG("[TM::writeTask] desc=[%s]", desc.c_str());

    if (desc.empty()) return;
    // initially just set last_time = ""
    dbh.insertTask(year, month, day, start_time, "", repeat, cat,
        priority, state, desc);
}

void taskManager::shift_IFidx(int ch)
{// be care of comparison of int and size_t
    // LOG("[TM::shift_IFidx] before: ifx=[%d], ify=[%d]", ifx, ify);

    // handle index change from type
    if (ifx==-1) { // type
        if (ch==KEY_UP || ch==KEY_DOWN) {
            type->setHovered(false);
            ifx = (ch==KEY_UP)?fields[typenum].size()-1:0;
            fields[typenum][ifx][ify]->setHovered(true);
        }
        // LOG("[TM::shift_IFidx] from type: ifx=[%d], ify=[%d]", ifx, ify);
        return;
    }

    // handle index change from fields
    fields[typenum][ifx][ify]->setHovered(false);
    if (ch == KEY_UP || ch == KEY_DOWN) {
        ifx = (ch == KEY_UP) ? ifx-1 : ifx+1;

        // check ifx out of range
        if (ifx >= (int)fields[typenum].size()) {
            // ifx = ifx % (int)fields.size();
            ifx = -1;
        } else if (ifx < -1) {
            ifx = (ifx + fields[typenum].size() + 1) % fields[typenum].size();
        }

        // going to type, return
        if (ifx==-1) {
            ify=0;
            type->setHovered(true);
            // LOG("[TM::shift_IFidx] going to type, return");
            return;
        }

        // in case ify out of range due to changed row
        if (ify >= (int)fields[typenum][ifx].size()) {
            ify = (int)fields[typenum][ifx].size()-1;
        }
    } else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
        ify = (ch == KEY_LEFT) ? ify-1 : ify+1;
        // check ify out of range
        if (ify >= (int)fields[typenum][ifx].size()) {
            ify = ify % (int)fields[typenum][ifx].size();
        } else if (ify < 0) {
            ify = (ify + fields[typenum][ifx].size()) % fields[typenum][ifx].size();
        }
    }
    fields[typenum][ifx][ify]->setHovered(true);
    // LOG("[TM::shift_IFidx] after: ifx=[%d], ify=[%d]", ifx, ify);
}

taskManager::taskManager(int y, int x, int h, int w)
    : h(h),w(w),inIF(false), printMap(3), fields(3), typenum(0)
{
    this->y = y;
    this->x = x;
    this->title = "Task Manager";

    for (int i=0; i<=2; i++) printMap[i].push_back(" Type: ");
    printMap[0].push_back(" Date:          /  /     Start:   : ");
    printMap[1].push_back("");
    printMap[2].push_back(" Date:          /  /     Start:   :     Repeat every: ");
    
    for (int i=0; i<=2; i++) {
        printMap[i].push_back(" Category:  ");
        printMap[i].push_back(" Priority:               State: ");
        printMap[i].push_back(" Desc:     ");
    }

    init_fields();
    // LOG("[taskManager::taskManager] after init_fields: (%d, %d)", y, x);
}

taskManager::~taskManager()
{
    LOG("[~taskManager] called");
}

int taskManager::handleOp(int ch)
{
    // LOG("[taskManager::handleOp] ch=[%d]", ch);
    int res = 0;
    if (inIF) {
        LOG("[taskManager::handleOp] sending into fields");
        if (ifx==-1) res = type->handleOp(ch);
        else res = fields[typenum][ifx][ify]->handleOp(ch);
    }

    // following are !inIF
    else if (isEnter(ch)) {
        // TODO: if is "enter", do addtask()
        LOG("[taskManager::handleOp] setting selected");
        if (ifx==-1) res=type->setSelected(true);
        else res=fields[typenum][ifx][ify]->setSelected(true);
        
        if (res==0) inIF = true;
    } else if (isArrow(ch)) {
        shift_IFidx(ch);
    }
    handleRC(res);
    return res;
}
void taskManager::handleRC(int res) {
    if (res==TM_STOP_IF) {
        if (ifx==-1) type->setSelected(false);
        else fields[typenum][ifx][ify]->setSelected(false);
        inIF = false;
    } else if (res==TM_TYPE_INC) {
        typenum = (typenum+1)%3;
    } else if (res==TM_TYPE_DEC) {
        typenum = (typenum-1+3)%3;
    } else if (res==TM_ENTER_PRESS) {
        writeTask();
    }
}
void taskManager::print()
{
    SubModule::print();
    mvprintwColor(y, x+1+title.size(), " / Expense Manager", 3);

    // LOG("[taskManager::print] init (%d, %d)", y, x);

    type->print();
    int py = y+2;
    for (int i=0; i<printMap[typenum].size(); i++) {
        mvprintw(py++, x, printMap[typenum][i].c_str());
    }

    // print fields
    for (auto line : fields[typenum]) {
        for (auto field : line) field->print();
    }
}

std::string taskManager::getIFValue(std::string name)
{
    for (int row=0; row<fields[typenum].size(); row++) {
        for (int col=0; col<fields[typenum][row].size(); col++) {
            if (fields[typenum][row][col]->getname() == name) {
                return fields[typenum][row][col]->getVal();
            }
        }
    }
    LOG("[getIFValue] cannot find [%s] in fields", name.c_str());
    return std::string("");
}