#ifndef INPUTFIELD_H
#define INPUTFIELD_H
#include "tool.h"

class inputField
{
protected:
    int y, x;
    // int h, w; // only desc need this?
    std::string name;
    bool hovered;
    bool selected; // 
    int valIdx;
    std::vector<std::string> vals;
    std::string dfval; // only acptTyping inputFields will use this
    bool acptTyping;
    int cursorIdx;

    // for switching value
    int switchV(int i);

    // for modifying value
    int handleDescOp(int ch);
    void insCh(int ch);
    void bsCh();
    void delCh();
    void shiftCurs(int i);
public:
    inputField(int y, int x, std::string n, std::string df="");
    ~inputField();

    int gety();
    int getx();
    std::string getname();
    void setHovered(bool v);
    int setSelected(bool v);
    int handleOp(int ch);
    void setValRange(int newRange); // only for "day"
    int setVal(std::string);
    std::string getVal();
    std::string getDV(); // display val
    void print();
};

#endif