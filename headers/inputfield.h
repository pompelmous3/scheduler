#ifndef INPUTFIELD_H
#define INPUTFIELD_H
#include "tool.h"

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

#endif