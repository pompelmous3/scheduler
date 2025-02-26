#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include "db.h"
#include "screenobject.h"
#include "submodule.h"
#include "displayfield.h"
#include "inputfield.h"
#include "tool.h"
#include <string>
#include <vector>
#include <memory>


class taskManager : public SubModule
{
private:
    // std::vector<std::vector<std::shared_ptr<displayField>>> fields;
    std::shared_ptr<displayField> type;
    int typenum; // for indexing in fields?
    // storing objects' pointer, shouldn't have slicing problem
    std::vector<std::vector<std::vector<std::shared_ptr<displayField>>>> fields;
    std::shared_ptr<displayField> yearField;
    std::shared_ptr<displayField> monthField;
    std::shared_ptr<displayField> dayField;
    std::shared_ptr<displayField> hourField;
    std::shared_ptr<displayField> minField;
    std::shared_ptr<displayField> repeatField;
    std::shared_ptr<displayField> categoryField;
    std::shared_ptr<displayField> priorityField;
    std::shared_ptr<displayField> stateField;
    std::shared_ptr<inputField> descField;
    std::shared_ptr<displayField> enterField;

    /*
    fields[i][y][x] means the 2D inputField vectors for type i
    i=0: date specific tasks
    i=1: general tasks
    i=2: routine tasks
    */

    bool editing;
    int editing_tid;
    task_entry tk;

    std::shared_ptr<DBHandler> dbh;
    int ifx, ify; // (-1,0) means type, else is index in fields[i]
    // int selected; // if selected, the curPos need to print in another color
    int h,w;
    std::vector<std::vector<std::string>> printMap;
    bool inIF;
    void init_fields();
    // int getIFColor(int row, int col);
    void writeTask();
    void shift_IFidx(int ch);

public:
    taskManager(int y, int x, int h, int w, std::shared_ptr<DBHandler>);
    ~taskManager() override;
    int handleOp(int ch) override;
    void handleRC(int& res);
    void print() override;
    void putTask(int tid);
    void setEdit(bool v, int tid);
    std::string getIFValue(std::string name);

    /*
    query latest ALL cats from dbh, send into catIF
    1. 1st time init TM
    2. When CM add/delete task cat
    */
    void updateCatIF();
};

#endif