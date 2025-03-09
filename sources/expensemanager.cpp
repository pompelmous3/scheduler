#include "expensemanager.h"

expenseManager::expenseManager(int y_, int x_, int h_, int w_) 
    : h(h_), w(w_)
{
    this->y = y_;
    this->x = x_;
    this->title = "               Expense Manager";

    // init_fields()
}

expenseManager::~expenseManager() {}

int expenseManager::handleOp(int ch)
{
    int rc = 0;
    LOG("[expenseManager::handleOp] ch=[%d]", ch);
    return rc;
}

void expenseManager::print() {
    SubModule::print();
    mvprintwColor(y, x+1, "Task Manager /", 3);
}