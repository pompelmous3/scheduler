#ifndef ADDTASKPANEL_H
#define ADDTASKPANEL_H
#include "db.h"
#include "screenobject.h"
#include "tool.h"
#include "string.h"
#include <string>
#include <vector>
#include <memory>

class inputField
{
protected:
    int sc_y, sc_x;
    int cursorIdx;
    std::string name;
    std::string strValue;
    int entering;
    int typing;
    bool forAction;

public:
    inputField(int y, int x, std::string n);
    ~inputField();
    virtual void switchV(int i) = 0;
    // void readInput();
    int gety();
    int getx();
    std::string getname();
    virtual std::string getv() = 0;
    virtual void setv(int ch) = 0;
    virtual void backspacev() = 0;
    virtual void deletev() = 0;
    virtual void shiftCurs(int i) = 0;
    int getTyping();
    int getCursorIdx();
    int geten();
    bool getAction();
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
    void setv(int ch) override;
    void backspacev() override;
    void deletev() override;
    void shiftCurs(int i) override;
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
    void setv(int ch) override;
    void backspacev() override;
    void deletev() override;
    void shiftCurs(int i) override;
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
    int enterMode;
    int typingMode;
    void init_inputFields();
    int getIFColor(int row, int col);
    std::optional<std::pair<int, int>> print_inputFields();
    void addTask();
    void shift_curPos(int ch);

public:
    addTaskPanel(int sc_h, int sc_w);
    ~addTaskPanel();
    int handleOp(int ch);
    std::optional<std::pair<int, int>> print();
    std::string getIFValue(std::string name);
};

#endif