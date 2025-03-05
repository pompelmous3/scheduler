#include "categorymanager.h"
#include "tool.h"
#include "return_code.h"

categoryManager::categoryManager(int y, int x, int h, int w,
    std::shared_ptr<DBHandler> dbh)
    : cat_idx(0), in_new_cat(false) {
    this->y = y;
    this->x = x;
    this->title = "Categories";
    this->h = h;
    this->w = w;
    COLSZ = h-2;

    new_cat = std::make_shared<inputField>(this->y, this->x+26, "new_cat",w-26,1);
    this->dbh = dbh;
    updateVals();
}

categoryManager::~categoryManager() {}

int categoryManager::handleOp(int ch)
{
    int res = 0;

    if (in_new_cat) res=new_cat->handleOp(ch);

    if (isArrow(ch)) arrowOp(ch);
    else if (isEnter(ch)) {
        if (cat_idx==-1) {
            new_cat->setSelected(true);
            in_new_cat = true;
        } else {
            vals[cat_idx].active++;
            vals[cat_idx].active %= 2;
            // update to =>dbh =>db
            dbh->updateCat(vals[cat_idx].cname, vals[cat_idx].active);
            res = CAT_UPDATED;
        }
    } else if (isCtrlD(ch) && cat_idx!=-1) {
        dbh->removeCat(vals[cat_idx].cname);
        res = CAT_UPDATED;
        vals.erase(vals.begin()+cat_idx);
        cat_idx %= vals.size();
    }

    handleRC(res);
    return res;
}

void categoryManager::handleRC(int& rc)
{
    if (rc==MNGR_STOP_IF) {
        new_cat->setSelected(false);
        in_new_cat = false;
        /*
        1. INSERT new category
        2. clear new_cat
        */
        std::string ncat = new_cat->getVal();
        LOG("[CM::handleRC] inserting new cat =[%s]", ncat.c_str());
        if (ncat != "") {
            dbh->insertCat(ncat);
            rc = CAT_UPDATED;
            // new_cat->setVal("");
            new_cat->clear();
        }
    }

    if (rc==CAT_UPDATED) updateVals();
}

void categoryManager::arrowOp(int ch)
{
    LOG("[CM::arrowOp] before, cat_idx=[%d]", cat_idx);

    /*
    If the move will cause a transition between new_cat and cat vals, we 
        should handle first.
    
    * transition from cat vals to new_cat
    1. if cat_idx is at top row, and UP received
      => move cat_idx to -1
    2. if cat_idx is at bottom row of that col, and DOWN received
      => move cat_idx to -1

    * transition from new_cat to cat vals
    3. if cat_idx==-1, UP received
      => move to 1st column last row
    4. if cat_idx==-1, DOWN received
      => move cat_idx to 0
    */
    int cur_col = cat_idx/COLSZ;
    int cur_row = cat_idx%COLSZ;
    bool trans_case = true;

    if (cur_row==0 && ch==KEY_UP) { // case 1
        LOG("[CM::arrowOp] case1");
        cat_idx = -1;
        new_cat->setHovered(true);
    } else if ((cur_row==COLSZ-1 || cat_idx+1==vals.size()) && ch==KEY_DOWN) { // case 2
        LOG("[CM::arrowOp] case2");
        cat_idx = -1;
        new_cat->setHovered(true);
    } else if (cat_idx==-1 && ch==KEY_UP) { // case 3
        LOG("[CM::arrowOp] case3");
        cat_idx = (vals.size()>=COLSZ)?(COLSZ-1):(vals.size()-1);
        new_cat->setHovered(false);
    } else if (cat_idx==-1 && ch==KEY_DOWN) { // case 4
        LOG("[CM::arrowOp] case4");
        cat_idx = 0;
        new_cat->setHovered(false);
    } else {
        trans_case = false;
    }

    if (trans_case) return;
    // when cat_idx==-1, we only need to handle trans_cases
    if (cat_idx==-1) return;

    int col_idx, row_idx;
    if (ch==KEY_UP || ch==KEY_DOWN) {
        col_idx = (cat_idx/COLSZ);
        row_idx = (cat_idx%COLSZ);
        // how many categories are in this col
        int max_row_num = (vals.size()>(COLSZ*(col_idx+1)-1))?
                        COLSZ: ((vals.size()-1)%COLSZ + 1);
        row_idx = (row_idx+ (ch==KEY_UP?-1:1)) % max_row_num;
        if (row_idx<0) row_idx=max_row_num-1;
        cat_idx = row_idx + COLSZ*col_idx;
    } else if (ch==KEY_LEFT || ch==KEY_RIGHT) {
        int max_col_num = (vals.size())/COLSZ + 1;
        col_idx = ((cat_idx/COLSZ)+(ch==KEY_LEFT?-1:1))%max_col_num;
        if (col_idx<0) col_idx=max_col_num-1;
        cat_idx = (cat_idx%COLSZ) + COLSZ*col_idx;
        // if last column doesn't have same amount of rows, shift again
        // -- right: shift to col 0
        // -- left: shift to left 1 col
        if (cat_idx>=vals.size()) cat_idx=(ch==KEY_RIGHT)?
            (cat_idx%COLSZ):(cat_idx-COLSZ);
    }

    LOG("[CM::arrowOp] after, cat_idx=[%d]", cat_idx);
}

void categoryManager::print() {
    SubModule::print();
    new_cat->print();
    mvprintw(this->y, this->x+17, "add new: ");

    // print categories
    int py = this->y+2;
    int px = this->x+4;
    int max_col_x = 0;
    for (int i=0; i<vals.size(); i++) {

        if (i!=0 && (i%COLSZ == 0)) { // time to change column
            px += max_col_x + CAT_MARGIN;
            max_col_x = 0;
            py = this->y+2;
        }

        if (vals[i].active) mvprintw(py, px, u8"\u25A3");
        else mvprintw(py, px, u8"\u25A2");

        // if this submodule is hovered, we do display cursor on selected category
        if (hovered && i==cat_idx)
            mvprintwColor(py++, px+BX_SZ, vals[i].cname.c_str(), 108);
        else
            mvprintw(py++, px+BX_SZ, vals[i].cname.c_str());
        max_col_x = std::max(max_col_x, (int)vals[i].cname.size()+BX_SZ);
    }


}

void categoryManager::updateVals()
{
    vals = dbh->queryTaskCats();
}