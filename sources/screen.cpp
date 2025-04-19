#include "screen.h"
#include "calendar.h"
#include "log.h"
#include "return_code.h"

#include <chrono>
#include <ctime>

Screen::Screen()
	: mode(0), tm_em(0), delegESC(false), lockTab(false)
{
	// single instance, shared across multiple (sub)modules
	dbh = std::make_shared<DBHandler>("./scheduler.db");

	// initialize screen height, width
	int h, w;
	getTerminalSize(h, w);
	// LOG("[Screen::Screen] (h,w)=(%d,%d)", h, w);
	// TODO: if w/h is smaller then MIN, exit?

	// x_char = L'\u2550'; // ═
	// y_char = L'\u2551'; // ║
	// tl_char = L'\u2554'; // ╔
	// tr_char = L'\u2557'; //  ╗
	// bl_char = L'\u255A'; // ╚
	// br_char = L'\u255D'; //  ╝
	// mdn_char = L'\u2566'; // ╦
	// mup_char = L'\u2569'; // ╩
	// mlft_char = L'\u2563'; // ╣
	// mrt_char = L'\u2560'; // ╠

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
		MIN_CAL_H,((sc_w-SC_PADDING_X*2-1)/2)-1, dbh);
	submods.push_back(calendar);
	dateSpecificTasks = std::make_shared<taskPanel>(cal_end_y+2, left_x+1,
		bottom_y-1-(cal_end_y+1), mid_x-left_x-1, std::string("Date Tasks"),
		dbh);
	submods.push_back(dateSpecificTasks);
	tm = std::make_shared<taskManager>(mng_top+1, mid_x+1, MNG_HEIGHT,
		right_x-mid_x-1, dbh);
	submods.push_back(tm);
	// em = std::make_shared<expenseManager>(mng_top+1, mid_x+1, MNG_HEIGHT,
	// 	right_x-mid_x-1);
	// submods.push_back(em);
	cm = std::make_shared<categoryManager>(tm_bottom+1, mid_x+1, MNG_HEIGHT,
		right_x-mid_x-1, dbh);
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
	int rc = 0;
	while(1)
	{
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
	// std::tm* curT = curTimeCompnt();
}

void Screen::printFrame() {
	wchar_t x_char = L'\u2550'; // ═
	wchar_t y_char = L'\u2551'; // ║
	wchar_t tl_char = L'\u2554'; // ╔
	wchar_t tr_char = L'\u2557'; //  ╗
	wchar_t bl_char = L'\u255A'; // ╚
	wchar_t br_char = L'\u255D'; //  ╝
	wchar_t mdn_char = L'\u2566'; // ╦
	wchar_t mup_char = L'\u2569'; // ╩
	wchar_t mlft_char = L'\u2563'; // ╣
	wchar_t mrt_char = L'\u2560'; // ╠


	// TOP
	for (int xsft=left_x; xsft<=right_x; xsft++) {
		mvaddnwstr(top_y, xsft, &x_char, 1);
	}
	// BOTTOM
	for (int xsft=left_x; xsft<=right_x; xsft++) {
		mvaddnwstr(bottom_y, xsft, &x_char, 1);
	}
	// LEFT
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvaddnwstr(top_y+ysft, left_x, &y_char, 1);
	}
	// RIGHT
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvaddnwstr(top_y+ysft, right_x, &y_char, 1);
	}

	// TOP-LEFT
	mvaddnwstr(SC_PADDING_Y, SC_PADDING_X, &tl_char, 1);
	// TOP-RIGHT
	mvaddnwstr(SC_PADDING_Y, right_x, &tr_char, 1);
	// BOTTOM-LEFT
	mvaddnwstr(sc_h-1-SC_PADDING_Y, SC_PADDING_X, &bl_char, 1);
	// BOTTOM-RIGHT
	mvaddnwstr(sc_h-1-SC_PADDING_Y, right_x, &br_char, 1);


	// middle vertical line
	for (int ysft=0; ysft<fr_h; ysft++) {
		mvaddnwstr(SC_PADDING_Y+ysft, mid_x, &y_char, 1);
	}
	mvaddnwstr(SC_PADDING_Y+0, mid_x, &mdn_char, 1);
	mvaddnwstr(SC_PADDING_Y+fr_h-1, mid_x, &mup_char, 1);

	// horizontal line under Calendar
	mvaddnwstr(cal_end_y+1, SC_PADDING_X, &mrt_char, 1);
	for (int xsft=1; xsft<fr_w/2; xsft++) {
		mvaddnwstr(cal_end_y+1, SC_PADDING_X+xsft, &x_char, 1);
	}
	mvaddnwstr(cal_end_y+1, SC_PADDING_X+fr_w/2, &mlft_char, 1);


	// top of task manager
	for (int xsft=mid_x; xsft<=right_x; xsft++) {
		mvaddnwstr(mng_top, xsft, &x_char, 1);
	}
	mvaddnwstr(mng_top, mid_x, &mrt_char, 1);
	mvaddnwstr(mng_top, right_x, &mlft_char, 1);

	// bottom of task manager
	for (int xsft=mid_x; xsft<=right_x; xsft++) {
		mvaddnwstr(tm_bottom, xsft, &x_char, 1);
	}
	mvaddnwstr(tm_bottom, mid_x, &mrt_char, 1);
	mvaddnwstr(tm_bottom, right_x, &mlft_char, 1);
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
	} else if (rc==CAT_UPDATED) {
		/*
		1. No need to update month scheduledDays because already updated
			at every print.
		2. update dateSpecificTasks
		3. update TM->categoryField
		*/
		update_dateSpecificTasks();
		tm->updateCatIF();
	}

}

void Screen::update_dateSpecificTasks() {
	std::vector<int> date = calendar->getDate();
	if (!date[3]) return; // {y,m,d,selected}
	dateSpecificTasks->updateTasks(date[0],date[1],date[2]);
}