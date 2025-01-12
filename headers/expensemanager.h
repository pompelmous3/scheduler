#ifndef EXPENSEMANAGER_H
#define EXPENSEMANAGER_H
#include "tool.h"
#include "submodule.h"
#include "inputfield.h"

class expenseManager : public SubModule
{
private:
    std::vector<std::vector<std::shared_ptr<inputField>>> fields;
    int h, w;
public:
    expenseManager(int y, int x, int h, int w);
    ~expenseManager();
    int handleOp(int ch) override;
    void print() override;
};

#endif