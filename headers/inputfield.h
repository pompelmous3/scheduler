#ifndef INPUTFIELD_H
#define INPUTFIELD_H
#include "displayfield.h"
#include "tool.h"

class inputField : public displayField
{
private:
    // bool acptTyping;
    std::string dfval; // only acptTyping inputFields will use this

    void insCh(int ch);
    void bsCh();
    void delCh();
    void shiftCurs(int i);
public:
    inputField(int, int, std::string);
    ~inputField();
    int handleOp(int) override;
    // set vals[0] val
    int setVal(std::string) override;
    std::string getDV() override;
    void clear();
    void print() override;
};
#endif