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

    /*
    fields[i][y][x] means the 2D inputField vectors for type i
    i=0: date specific tasks
    i=1: general tasks
    i=2: routine tasks
    */

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
    void handleRC(int res);
    void print() override;
    std::string getIFValue(std::string name);
};

#endif