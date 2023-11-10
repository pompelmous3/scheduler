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
    int selected;
} inputField;


// TODO: change inputField to class
// 1. when entering, inputField can use UP/DOWN to modify value 
//      if value is number-like
// 2. when entering, inputField can read typed string from screen


class addTaskPanel : public ScreenObject
{
private:
    std::vector <std::vector<inputField>> inputFields;
    // inputFields[y][x] means the inputField which is at y row and x column

    DBHandler dbh = DBHandler("/var/log/scheduler.db");
    std::pair<int, int> curPos; // current inputField pos
    int selected; // if selected, the curPos need to print in another color
    int entering = 0;
    void init_inputFields();
    int getIFColor(int row, int col);
    void print_inputFields();

public:
    addTaskPanel(int sc_h, int sc_w);
    ~addTaskPanel();
    void handleOp(int ch);
    void print();
};

#endif