#ifndef ADDTASKPANEL_H
#define ADDTASKPANEL_H
#include "db.h"
#include "screenobject.h"
#include "tool.h"
#include "string.h"
#include <string>
#include <vector>
#include <memory>

// typedef struct {
//     int sc_y;
//     int sc_x;
//     std::string value;
//     int selected;
// } inputField;

class inputField
{
    int sc_y, sc_x;
    int optIdx;
    std::string name;
    int entering;

public:
    inputField(int y, int x, std::string n);
    ~inputField();
    virtual void switchV(int i) = 0; // diff in intIF/strIF
    // void readInput();
    int gety();
    int getx();
    virtual std::string getv() = 0; // diff in intIF/strIF
    int geten();
    void toggleen();
};

class intIF : public inputField
{
    int value;
    int fixedLen;
    int upperBnd;
    int lowerBnd;

    // TODO: set upper/lower bound
public:
    intIF(int y, int x, std::string n, int val);
    ~intIF();
    std::string getv() override;
    void switchV(int i) override;
};

class strIF : public inputField
{
    // std::string value;
    int idx;
    std::vector<std::string> opts;
public:
    strIF(int y, int x, std::string n);
    ~strIF();
    std::string getv() override;
    void switchV(int i) override;
};

// TODO: change inputField to class
// 1. when entering, inputField can use UP/DOWN to modify value 
//      if value is number-like
// 2. when entering, inputField can read typed string from screen
// 3. if entering in some inputField,


class addTaskPanel : public ScreenObject
{
private:
    // std::vector <std::vector<inputField>> inputFields;
    std::vector<std::vector<std::shared_ptr<inputField>>> inputFields;
    // inputFields[y][x] means the inputField which is at y row and x column

    DBHandler dbh = DBHandler("/var/log/scheduler.db");
    std::pair<int, int> curPos; // current inputField pos
    std::pair<int, int> enterPos; // entering inputField pos
    // int selected; // if selected, the curPos need to print in another color
    int enterMode = 0;
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