#ifndef SUBMODULE_H
#define SUBMODULE_H
class SubModule {

public:
    SubModule(){};
    ~SubModule(){};
    virtual int handleOp(int) = 0;
    virtual void print() = 0;
};
#endif