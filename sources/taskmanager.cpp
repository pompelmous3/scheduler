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
    

    // ################ line 1 ################
    std::vector<std::shared_ptr<inputField>> lineOne;
    std::shared_ptr<inputField> year = std::make_shared<inputField>(
        y+2,
        x+12,
        std::string("year"),
        toStr(cyr, 4)
    );
    lineOne.push_back(std::move(year));

    std::shared_ptr<inputField> month = std::make_shared<inputField>(
        y+2,
        x+17,
        std::string("month"),
        toStr(cm, 2)
    );
    lineOne.push_back(std::move(month));

    std::shared_ptr<inputField> day = std::make_shared<inputField>(
        y+2,
        x+20,
        std::string("day"),
        toStr(dofm[cm-1],2)+toStr(cd, 2)
    );
    lineOne.push_back(std::move(day));
    // fields.push_back(std::move(lineOne));

    std::shared_ptr<inputField> hour = std::make_shared<inputField>(
        y+2,
        x+32,
        std::string("hour"),
        toStr(ch, 2)
    );
    lineOne.push_back(std::move(hour));

    std::shared_ptr<inputField> min = std::make_shared<inputField>(
        y+2,
        x+35,
        std::string("min"),
        toStr(cmin, 2)
    );
    lineOne.push_back(std::move(min));
    fields.push_back(lineOne);

    // ################ line 3  ################
    std::vector<std::shared_ptr<inputField>> lineThree;
    std::shared_ptr<inputField> category = std::make_shared<inputField>(
        y+4,
        x+12,
        std::string("category")
    );
    lineThree.push_back(std::move(category));
    fields.push_back(lineThree);

    // ################ line 4 ################
    std::vector<std::shared_ptr<inputField>> lineFour;
    std::shared_ptr<inputField> priority = std::make_shared<inputField>(
        y+5,
        x+12,
        std::string("priority")
    );
    lineFour.push_back(std::move(priority));
    fields.push_back(lineFour);

    // ################ line 5 ################
    std::vector<std::shared_ptr<inputField>> lineFive;
    std::shared_ptr<inputField> desc = std::make_shared<inputField>(
        y+6,
        x+12,
        std::string("desc")
    );
    lineFive.push_back(std::move(desc));
    fields.push_back(lineFive);

    // ################ line 7 ################
    std::vector<std::shared_ptr<inputField>> lineSeven;
    std::shared_ptr<inputField> enter = std::make_shared<inputField>(
        y+8,
        x+2+(w-4)/2-2,
        std::string("enter")
    );
    lineSeven.push_back(std::move(enter));
    fields.push_back(lineSeven);

    // curPos = std::make_pair(0, 0);
    ifx = 0; ify = 0;
    fields[ifx][ify]->setHovered(true);
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
    std::string cls = getIFValue(std::string("category"));
    std::string priority = getIFValue(std::string("priority"));
    std::string desc = getIFValue(std::string("description"));
    dbh.insertTask(year, month, day, start_time, cls, priority, desc);
}

void taskManager::shift_IFidx(int ch)
{// be care of comparison of int and size_t
    fields[ifx][ify]->setHovered(false);
    if (ch == KEY_UP || ch == KEY_DOWN) {
        ifx = (ch == KEY_UP) ? ifx-1 : ifx+1;

        // check ifx out of range
        if (ifx >= (int)fields.size()) {
            ifx = ifx % (int)fields.size();
        } else if (ifx < 0) {
            ifx = (ifx + fields.size()) % fields.size();
        }

        // in case ify out of range due to changed row
        if (ify >= (int)fields[ifx].size()) {
            ify = (int)fields[ifx].size()-1;
        }
    } else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
        ify = (ch == KEY_LEFT) ? ify-1 : ify+1;
        // check ify out of range
        if (ify >= (int)fields[ifx].size()) {
            ify = ify % (int)fields[ifx].size();
        } else if (ify < 0) {
            // ify = ((int)fields[ifx].size()-1)- (0-ify-1);
            ify = (ify + fields[ifx].size()) % fields[ifx].size();
        }
    }
    fields[ifx][ify]->setHovered(true);
}

taskManager::taskManager(int y, int x, int h, int w)
    : h(h),w(w),inIF(false)
{
    this->y = y;
    this->x = x;
    this->title = "Task Manager";
    printMap.push_back(" Date:          /  /     Start:   :     Repeat: ");
    // printMap.push_back(" Start Time:    :  ");
    printMap.push_back(" Category:  ");
    printMap.push_back(" Priority:     ");
    printMap.push_back(" Desc:     ");

    init_fields();
    // LOG("[taskManager::taskManager] after init_fields: (%d, %d)", y, x);
}

taskManager::~taskManager()
{
    LOG("[~taskManager] called");
}

int taskManager::handleOp(int ch)
{
    LOG("[taskManager::handleOp] ch=[%d]", ch);
    int res = 0;
    if (inIF) {
        LOG("[taskManager::handleOp] sending into fields");
        res = fields[ifx][ify]->handleOp(ch);
    } else if (isEnter(ch)) {
        // TODO: if is "enter", do addtask()
        LOG("[taskManager::handleOp] setting selected");
        fields[ifx][ify]->setSelected(true);
        inIF = true;
    } else if (isArrow(ch)) {
        shift_IFidx(ch);
    }
    handleRC(res);
    return res;

    // if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_RIGHT || ch==KEY_LEFT
    //     || ch==KEY_END || ch==KEY_HOME) {
    //     if (typingMode) {
    //         switch (ch) {
    //         case KEY_LEFT:
    //             fields[enterPos.first][enterPos.second]->shiftCurs(-1);
    //             break;
    //         case KEY_RIGHT:
    //             fields[enterPos.first][enterPos.second]->shiftCurs(1);
    //             break;
    //         case KEY_HOME:
    //             fields[enterPos.first][enterPos.second]->shiftCurs(0-w);
    //             break;
    //         case KEY_END:
    //             fields[enterPos.first][enterPos.second]->shiftCurs(w);
    //             break;
    //         }
    //     } else if (enterMode) { // one of the fields is selected, do switch
    //         switch (ch) {
    //         case KEY_UP:
    //             fields[enterPos.first][enterPos.second]->switchV(1);
    //             break;
    //         case KEY_DOWN:
    //             fields[enterPos.first][enterPos.second]->switchV(-1);
    //             break;
    //         default:
    //             break;
    //         }
    //     } else {
    //         shift_curPos(ch);
    //     }
    // }

    // else if (ch == KEY_ENTER) {
    //     if (fields[ifx][ify]->getAction()) {
    //         addTask();
    //         return STOP_SC_ATPMODE;
    //     } else {
    //         fields[ifx][ify]->toggleen();
    //         enterMode = (enterMode + 1) % 2;
    //         if (enterMode == 0) {
    //             typingMode = 0;
    //         }
    //         enterPos.first = ifx;
    //         enterPos.second = ify;
    //     }
    // } else if (ch == KEY_BACKSPACE) { // Backspace
    //     if (enterMode) {
    //         fields[ifx][ify]->backspacev();
    //     }
    // } else if (ch == KEY_DC) {
    //     if (enterMode) {
    //         fields[ifx][ify]->deletev();
    //     }
    // } else { // not ARROW/ENTER/BACKSPACE
    //     if (enterMode) {
    //         typingMode = 1;
    //         fields[ifx][ify]->setv(ch);
    //     }
    // }
}
void taskManager::handleRC(int res) {
    if (res==TM_STOP_IF) {
        fields[ifx][ify]->setSelected(false);
        inIF = false;
    }
}
void taskManager::print()
{
    SubModule::print();
    mvprintwColor(y, x+1+title.size(), " / Expense Manager", 3);

    // LOG("[taskManager::print] init (%d, %d)", y, x);

    int py = y+2;
    for (int i=0; i<printMap.size(); i++) {
        mvprintw(py++, x, printMap[i].c_str());
    }

    // print fields
    for (auto line : fields) {
        for (auto field : line) field->print();
    }
}

std::string taskManager::getIFValue(std::string name)
{
    for (int row=0; row<fields.size(); row++) {
        for (int col=0; col<fields[row].size(); col++) {
            if (fields[row][col]->getname() == name) {
                return fields[row][col]->getVal();
            }
        }
    }
    LOG("[getIFValue] cannot find [%s] in fields", name.c_str());
    return std::string("");
}