#ifndef ADDTASKPANEL_H
#define ADDTASKPANEL_H
#include "db.h"
#include "screenobject.h"
#include "tool.h"
#include <string>
#include <vector>

typedef struct {
    int sc_y;
    int sc_x;
    std::string value;
} inputField;

class addTaskPanel : public ScreenObject
{
private:
    std::vector <std::vector<inputField>> inputFields;
    // inputFields[y][x] means the inputField which is at y row and x column

    DBHandler dbh = DBHandler("/var/log/scheduler.db");
    std::pair<int, int> curPos; // current inputField pos
    int selected; // if selected, the curPos need to print in another color
    void init_inputFields();
    void print_inputFields();

public:
    addTaskPanel(int sc_h, int sc_w);
    ~addTaskPanel();
    void handleOp(int ch);
    void print();
};

#endif