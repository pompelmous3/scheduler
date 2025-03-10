#ifndef CALENDAR_H
#define CALENDAR_H
#include "db.h"
#include "menu.h"
#include "tool.h"
#include "month.h"
#include "taskpanel.h"
#include "submodule.h"
#include <string> // need include here so we can use std::string
#include <vector>


#define CAL_PADDING 1
#define MIN_CAL_W 30
#define MIN_CAL_H 11


class Calendar : public SubModule {
    int h;
    int w;
    int max_mon_cnt;
    std::vector<std::shared_ptr<Month>> mons;
    size_t mon_idx;
    bool delegToMon; // If op need to delegate to mons (mons activated)
    std::shared_ptr<DBHandler> dbh;

public:
    Calendar(int, int, int, int, std::shared_ptr<DBHandler>);
    ~Calendar() override;
    void addMonth(std::shared_ptr<Month>);
    void shiftMonth(int);
    std::vector<int> getDate();
    int handleOp(int ch) override;
    void handleRC(int& rc);
    void print() override;

};

#endif