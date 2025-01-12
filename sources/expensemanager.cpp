#include "expensemanager.h"

expenseManager::expenseManager(int y, int x, int h, int w) 
    : h(h), w(w)
{
    this->y = y;
    this->x = x;
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
    mvprintw(y, x+1, "Task Manager /");
}