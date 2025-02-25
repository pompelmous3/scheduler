#include "screen.h"
#include "calendar.h"
#include "log.h"
#include "return_code.h"

#include <chrono>
#include <ctime>

Screen::Screen()
	: mode(0), tm_em(0), delegESC(false), lockTab(false)
{

	// only in Screen we check db tables
	dbh.initTables();

	// initialize screen height, width
	int h, w;
	getTerminalSize(h, w);
	// LOG("[Screen::Screen] (h,w)=(%d,%d)", h, w);
	// TODO: if w/h is smaller then MIN, exit?

	sc_h = h;
	sc_w = w;
	fr_w = sc_w-2*SC_PADDING_X;
	fr_h = sc_h-2*SC_PADDING_Y;

	top_y = SC_PADDING_Y;
	bottom_y = sc_h-1-SC_PADDING_Y; // sc_h-1 is the bottommost line
	left_x = SC_PADDING_X;
	right_x = sc_w-1-SC_PADDING_X; // sc_w-1 is the rightmost line
	mid_x = sc_w/2;
	cal_end_y = SC_PADDING_Y+1+MIN_CAL_H;

	// mng_top = top_y + (fr_h/8*3);
	mng_top = bottom_y -1 -MNG_HEIGHT -1 -MNG_HEIGHT;
	tm_bottom = mng_top + MNG_HEIGHT + 1;


	// initialize submodules
	calendar = std::make_shared<Calendar>(top_y+1, left_x+1,
		MIN_CAL_H,((sc_w-SC_PADDING_X*2-1)/2)-1);
	submods.push_back(calendar);
	dateSpecificTasks = std::make_shared<taskPanel>(cal_end_y+2, left_x+1,
		bottom_y-1-(cal_end_y+1), mid_x-left_x-1, std::string("Date Tasks"));
	submods.push_back(dateSpecificTasks);
	tm = std::make_shared<taskManager>(mng_top+1, mid_x+1, MNG_HEIGHT,
		right_x-mid_x-1);
	submods.push_back(tm);
	// em = std::make_shared<expenseManager>(mng_top+1, mid_x+1, MNG_HEIGHT,
	// 	right_x-mid_x-1);
	// submods.push_back(em);
	cm = std::make_shared<categoryManager>(tm_bottom+1, mid_x+1, MNG_HEIGHT,
		right_x-mid_x-1);
	submods.push_back(cm);


	submods[mode]->set_hovered(true);

	// First time print before any input
	printScr();

	// initial cursor y,x
	// TODO: Is this still in use?
	getyx(stdscr, cs_y, cs_x);
}

Screen::~Screen()
{
	LOG("[~Screen] destructor called");
}

void Screen::setMode(int m) {
	LOG("[SC::setMode] m = [%d]", m);
	submods[mode]->set_hovered(false);
	mode = m;
	if (mode<0) mode+=submods.size();
	mode %= submods.size();

	submods[mode]->set_hovered(true);
	
	// taskPanel
	std::shared_ptr<taskPanel> taskPtr = std::dynamic_pointer_cast<taskPanel>(submods[mode]);
	if (taskPtr) {
		taskPtr->setDisplayIdx(true);
	}

	// if new mode is tm/em, set tm_em/tm_em
	// if (submods[mode]==em) tm_em=1;
	// if (submods[mode]==tm) tm_em=0;
	
}
void Screen::looping() {
	int ch;
	int mvx, mvy;
	int rc = 0;
	while(1)
	{
		mvx = 0;
		mvy = 0;
		ch = getch();
		// LOG("[loopingMove] ch=[%d]",ch);

		if (ch==KEY_BTAB) {
			LOG("[looping] Get a KEY_BTAB");
		}

		rc = handleOp(ch);
		handleRC(rc);
		// LOG("[SC::looping] after handleRC, going to refresh");
		refreshScr();
		rc = 0;
	}
}

void Screen::timer_run() {
	/*
	1. get current date cy/cm/cd
	2. compare
	*/
	std::tm* curT = curTimeCompnt();
}

void Screen::printFrame() {
	std::string x_char = u8"\u2550";
	std::string y_char = u8"\u2551";
	std::string tl_char = u8"\u2554"; // top left
	std::string tr_char = u8"\u2557"; // top right
	std::string bl_char = u8"\u255A"; // bottom left
	std::string br_char = u8"\u255D"; // bottom right
	std::string mdn_char = u8"\u2566"; // v (middle, go down)
	std::string mup_char = u8"\u2569"; // ^ (middle, go up)
	std::string mlft_char = u8"\u2563"; // <=| (middle, go left)
	std::string mrt_char = u8"\u2560"; //    |=> (middle, go right)


	// TOP
	for (int xsft=left_x; xsft<=right_x; xsft++) {
		mvprintw(top_y, xsft, x_char.c_str());
	}
	// BOTTOM
	for (int xsft=left_x; xsft<=right_x; xsft++) {
		mvprintw(bottom_y, xsft, x_char.c_str());
	}
	// LEFT
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvprintw(top_y+ysft, left_x, y_char.c_str());
	}
	// RIGHT
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvprintw(top_y+ysft, right_x, y_char.c_str());
	}

	// TOP-LEFT
	mvprintw(SC_PADDING_Y, SC_PADDING_X, tl_char.c_str());
	// TOP-RIGHT
	mvprintw(SC_PADDING_Y, right_x, tr_char.c_str());
	// BOTTOM-LEFT
	mvprintw(sc_h-1-SC_PADDING_Y, SC_PADDING_X, bl_char.c_str());
	// BOTTOM-RIGHT
	mvprintw(sc_h-1-SC_PADDING_Y, right_x, br_char.c_str());


	// middle vertical line
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvprintw(SC_PADDING_Y+ysft, mid_x, y_char.c_str());
	}
	mvprintw(SC_PADDING_Y+0, mid_x, mdn_char.c_str());
	mvprintw(SC_PADDING_Y+fr_h-1, mid_x, mup_char.c_str());


	// horizontal line under Calendar
	mvprintw(cal_end_y+1, SC_PADDING_X, mrt_char.c_str());
	for (int xsft=1; xsft<fr_w/2; xsft++) {
		mvprintw(cal_end_y+1, SC_PADDING_X+xsft, x_char.c_str());
	}
	mvprintw(cal_end_y+1, SC_PADDING_X+fr_w/2, mlft_char.c_str());



	



	// top of task/expense manager
	for (int xsft=mid_x; xsft<=right_x; xsft++) {
		mvprintw(mng_top, xsft, x_char.c_str());
	}
	mvprintw(mng_top, mid_x, mrt_char.c_str());
	mvprintw(mng_top, right_x, mlft_char.c_str());

	// bottom of task/expense manager
	for (int xsft=mid_x; xsft<=right_x; xsft++) {
		mvprintw(tm_bottom, xsft, x_char.c_str());
	}
	mvprintw(tm_bottom, mid_x, mrt_char.c_str());
	mvprintw(tm_bottom, right_x, mlft_char.c_str());
}

void Screen::printScr()
{
	printFrame();
	calendar->print();
	dateSpecificTasks->print();
	tm->print();
	cm->print();

	// switch tm/em
	// if (!tm_em) tm->print();
	// else em->print();
}

void Screen::refreshScr()
{
	erase();
	printScr();
	refresh();
}

int Screen::handleOp(int ch) {
	if (!lockTab && (ch==KEY_TAB || ch==KEY_BTAB)) { // TAB only for Screen switching modes
		setMode(mode + ((ch==KEY_TAB)?1:-1));
	} else if (!delegESC && isESC(ch)) {
		LOG("[Screen::handleOp] opening setting panel");
		//TODO: setting panel?
		return 0;
	} else {
		return submods[mode]->handleOp(ch);
	}
	return 0;
}

void Screen::handleRC(int rc)
{
	// LOG("[Screen::handleRC] rc=[%d]", rc);
	if (rc==START_DELEGESC) {
		delegESC = true;
		if (mode==0) update_dateSpecificTasks();
	}
	else if (rc==STOP_DELEGESC) delegESC = false;
	else if (rc==SC_UPDATE_DATE_2_SUBM) {
		update_dateSpecificTasks();
	} else if (rc==SC_SWITCH_2_DS) {
		int n_mode = std::distance(submods.begin(),
			std::find(submods.begin(), submods.end(), dateSpecificTasks));
		LOG("[SC::handleRC] setting mode to [%d]", n_mode);
		setMode(n_mode);
	} else if (rc==TM_WRITE_TASK) {
		update_dateSpecificTasks();
	} else if (rc==TP_EDIT_REQ) {
		// use mode to distinguish which taskPanel is sending rc
		int tid = std::dynamic_pointer_cast<taskPanel>(submods[mode])->get_cur_taskid();
		LOG("[SC::handleRC] editing tid=[%d]", tid);
		tm->putTask(tid);
		tm->setEdit(true, tid);
		// set mode to tm
		setMode(std::distance(submods.begin(), 
			std::find(submods.begin(), submods.end(), tm)));
		// STOP tab until edit done
		lockTab = true;
	} else if (rc==SC_UNLOCK_TAB) {
		lockTab = false;
		update_dateSpecificTasks();
	}

}

void Screen::update_dateSpecificTasks() {
	std::vector<int> date = calendar->getDate();
	dateSpecificTasks->updateTasks(date[0],date[1],date[2]);
}