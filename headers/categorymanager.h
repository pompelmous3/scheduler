#ifndef CATEGORYMANAGER_H
#define CATEGORYMANAGER_H
#include "submodule.h"
#include "inputfield.h"
#include "displayfield.h"
#include "tool.h"
#include "db.h"

#define BX_SZ 2
#define MAX_ENT_SZ 30

class categoryManager : public SubModule
{
private:
    std::vector<task_cat> vals;
    /*
    We use cat_idx to indicate which category is currently hovered
    special case: -1 => for new_cat
    
    */
    int cat_idx;
    int h,w;
    int COLSZ;
    std::shared_ptr<inputField> new_cat;
    bool in_new_cat;


    DBHandler dbh = DBHandler("./scheduler.db");
public:
    categoryManager(int y, int x, int h, int w);
    ~categoryManager() override;
    int handleOp(int ch) override;
    void handleRC(int ch);
    void arrowOp(int ch);
    void print() override;
};

#endif
