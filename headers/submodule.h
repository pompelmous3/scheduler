#ifndef SUBMODULE_H
#define SUBMODULE_H
#include "tool.h"
#include "log.h"
class SubModule {

public:
    SubModule();
    virtual ~SubModule() = default;
    virtual int handleOp(int) = 0;
    virtual void print();
    void set_hovered(bool);

    int y, x;
    std::string title;
    bool hovered;
};
#endif