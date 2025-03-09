#include "calendar.h"
#include "log.h"
#include "return_code.h"
#include <iostream>
#include <curses.h>


// ########################## Calendar ##########################
Calendar::Calendar(int y_, int x_, int h_, int w_, std::shared_ptr<DBHandler> dbh_)
  : h{h_}, w{w_}, max_mon_cnt{0}, mon_idx{0}, delegToMon{false}
{
	this->y = y_;
	this->x = x_;
	this->title = "Calendar";
	std::chrono::system_clock::time_point now = 
		std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	struct tm *local_t = localtime(&tt);
	this->dbh = dbh_;

	int cur_yr, cur_mon;
	cur_yr = (*local_t).tm_year+1900;
	cur_mon = (*local_t).tm_mon+1;
	int mon_y, mon_x;
	mon_y = y + CAL_PADDING + 1; // 1 line for title
	mon_x = x + CAL_PADDING;

	/*
	## n = max_mon_cnt
	(w-2*CAL_PADDING) >= (n*MON_WIDTH + (n-1)*MON_MARGIN)
	(w-2*CAL_PADDING+MON_MARGIN) >= (n*(MON_WIDTH+MON_MARGIN))
	=> so we can get max_mon_cnt (n) by
	  (w-2*CAL_PADDING+MON_MARGIN) / (MON_WIDTH+MON_MARGIN)
	*/
	max_mon_cnt = (w-2*CAL_PADDING+MON_MARGIN) / (MON_WIDTH+MON_MARGIN);

	// make months center-aligned
	mon_x = x + (w - max_mon_cnt*MON_WIDTH - (max_mon_cnt-1)*MON_MARGIN)/2;
	for (int i=0; i<max_mon_cnt; i++) {
		std::shared_ptr<Month> mn = std::make_shared<Month>(cur_yr, cur_mon,
			mon_y, mon_x, dbh);
		addMonth(mn);

		cur_mon++;
		if (cur_mon>12) {
			cur_mon = 1;
			cur_yr++;
		}
		mon_x += MON_WIDTH + MON_MARGIN;
	}
}

Calendar::~Calendar() {}

void Calendar::addMonth(std::shared_ptr<Month>m) {
	mons.push_back(m);
	if (mons.size()==1) {
		m->setBrowsed(1);
		mon_idx = 0;
	}
}

void Calendar::shiftMonth(int v) {
	mons[mon_idx]->setBrowsed(0);
	if ((mon_idx==0 && v<0) || (mon_idx==mons.size()-1 && v>0)) {
		/*
		Here we already sure we need to remove/adding month and shift.
		we can simply use (v<0)/(v>0) to distinguish the direction.
		*/
		std::vector<int> pre_date = (v>0?mons.back():mons.front())->getDate();
		std::vector<int> pre_pos = (v>0?mons.back():mons.front())->getPos();
		int adding_yr = pre_date[0];
		int adding_mon = pre_date[1]+v;
		if (adding_mon>12) {
			adding_yr++;
			adding_mon %= 12;
		} else if (adding_mon<=0) {
			adding_yr--;
			adding_mon = (adding_mon+12);
		}
		mons.erase((v>0)? mons.begin() : mons.end()-1);
		for (auto& m : mons) m->shiftPos(-v);
		std::shared_ptr<Month> mn = std::make_shared<Month>(adding_yr,
			adding_mon, pre_pos[0], pre_pos[1], dbh);
		// addMonth(mn);
		mons.insert((v>0)? mons.end() : mons.begin(), mn);
	}
	else {
		mon_idx += v;
	}
	mons[mon_idx]->setBrowsed(1);
}

std::vector<int> Calendar::getDate() {
	return mons[mon_idx]->getDate();
}

int Calendar::handleOp(int ch) {

	int rc = 0;
	// LOG("[Calendar::handleOp] ch=[%d]", ch);
	// handle OP in Calendar
	if (!delegToMon && (isArrow(ch) || isEnter(ch))) {
		if (ch==KEY_LEFT) shiftMonth(-1);
		else if (ch==KEY_RIGHT) shiftMonth(1);
		else if (isEnter(ch)) {
			LOG("[Calendar::handleOp] setting mon selected");
			mons[mon_idx]->setSelected(true);
			delegToMon = true;
			rc = START_DELEGESC;
		}
		return rc;
	}

	// other not handled cases pass to mon
	rc = mons[mon_idx]->handleOp(ch);
	handleRC(rc);
	return rc;
}

void Calendar::handleRC(int& rc) {
	// LOG("[Calendar::handleRC] rc=[%d]", rc);
	if (rc==CAL_STOP_MON) {
		LOG("[Calendar::handleRC] CAL_STOP_MON");
		mons[mon_idx]->setSelected(false);
		delegToMon = false;
		rc = STOP_DELEGESC;
	}
}

void Calendar::print() {
	SubModule::print();
	// mons.size() == max_mon_cnt
	// LOG("[Calendar::print] (%d, %d)", y, x);
	for (size_t i=0; i<mons.size(); i++) {
		mons[i]->print();
	}
}