#ifndef DISPLAYFIELD_H
#define DISPLAYFIELD_H
#include "tool.h"

class displayField
{
protected:
    int y, x;
    int max_w, max_h;
    std::string name;
    bool hovered;
    bool selected; // 
    size_t valIdx;
    std::vector<std::string> vals;
    // std::string dfval; // only acptTyping inputFields will use this
    // bool acptTyping;
    size_t cursorIdx;

    // for switching value
    int switchV(int i);

    // for modifying value
    // int handleTypingOp(int ch);
    // void insCh(int ch);
    // void bsCh();
    // void delCh();
    // void shiftCurs(int i);
public:
    displayField(int y, int x, std::string n, std::string df="", int mw=-1, int mh=-1);
    ~displayField();

    int gety();
    int getx();
    std::string getname();
    void setHovered(bool v);
    int setSelected(bool v);
    virtual int handleOp(int ch);
    void setValRange(size_t newRange); // only for "day"

    // Find v in vals, set idx to v
    virtual int setVal(std::string v);
    // Replace whole vals to be new_vals
    void updateVals(std::vector<std::string> new_vals);
    std::string getVal();
    virtual std::string getDV(); // display val
    virtual void print();
};

#endif