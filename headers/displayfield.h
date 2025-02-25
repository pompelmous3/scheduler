#ifndef DISPLAYFIELD_H
#define DISPLAYFIELD_H
#include "tool.h"

class displayField
{
protected:
    int y, x;
    // int h, w; // only desc need this?
    std::string name;
    bool hovered;
    bool selected; // 
    int valIdx;
    std::vector<std::string> vals;
    // std::string dfval; // only acptTyping inputFields will use this
    // bool acptTyping;
    int cursorIdx;

    // for switching value
    int switchV(int i);

    // for modifying value
    // int handleTypingOp(int ch);
    // void insCh(int ch);
    // void bsCh();
    // void delCh();
    // void shiftCurs(int i);
public:
    displayField(int y, int x, std::string n, std::string df="");
    ~displayField();

    int gety();
    int getx();
    std::string getname();
    void setHovered(bool v);
    int setSelected(bool v);
    virtual int handleOp(int ch);
    void setValRange(int newRange); // only for "day"
    virtual int setVal(std::string);
    std::string getVal();
    virtual std::string getDV(); // display val
    virtual void print();
};

#endif