#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include "db.h"
#include "screenobject.h"
#include "submodule.h"
#include "inputfield.h"
#include "tool.h"
#include <string>
#include <vector>
#include <memory>


class taskManager : public SubModule
{
private:
    // std::vector<std::vector<std::shared_ptr<inputField>>> fields;
    std::shared_ptr<inputField> type;
    int typenum; // for indexing in fields?
    std::vector<std::vector<std::vector<std::shared_ptr<inputField>>>> fields;
    std::shared_ptr<inputField> yearField;
    std::shared_ptr<inputField> monthField;
    std::shared_ptr<inputField> dayField;
    std::shared_ptr<inputField> hourField;
    std::shared_ptr<inputField> minField;
    std::shared_ptr<inputField> repeatField;
    std::shared_ptr<inputField> categoryField;
    std::shared_ptr<inputField> priorityField;
    std::shared_ptr<inputField> stateField;
    std::shared_ptr<inputField> descField;
    std::shared_ptr<inputField> enterField;

    /*
    fields[i][y][x] means the 2D inputField vectors for type i
    i=0: date specific tasks
    i=1: general tasks
    i=2: routine tasks
    */

    bool editing;
    int editing_tid;
    task_entry tk;

    DBHandler dbh = DBHandler("./scheduler.db");
    int ifx, ify; // (-1,0) means type, else is index in fields[i]
    // int selected; // if selected, the curPos need to print in another color
    int h,w;
    std::vector<std::vector<std::string>> printMap;
    bool inIF;
    void init_fields();
    // int getIFColor(int row, int col);
    // std::optional<std::pair<int, int>> print_inputFields();
    void writeTask();
    void shift_IFidx(int ch);

public:
    taskManager(int y, int x, int h, int w);
    ~taskManager() override;
    int handleOp(int ch) override;
    void handleRC(int& res);
    void print() override;
    void putTask(int tid);
    void setEdit(bool v, int tid);
    std::string getIFValue(std::string name);
};

#endif