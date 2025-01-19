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
    std::vector<std::vector<std::shared_ptr<inputField>>> fields;
    // inputFields[y][x] means the inputField which is at y row and x column

    DBHandler dbh = DBHandler("./scheduler.db");
    int ifx, ify;
    // int selected; // if selected, the curPos need to print in another color
    int h,w;
    // int enterMode;
    // int typingMode;
    std::vector <std::string> printMap;
    bool inIF;
    void init_fields();
    // int getIFColor(int row, int col);
    // std::optional<std::pair<int, int>> print_inputFields();
    void addTask();
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