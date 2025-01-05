#include "../headers/screen.h"
#include "../headers/calendar.h"
#include "../headers/log.h"

#include <chrono>
#include <ctime>

Screen::Screen()
	: mode(0), delegESC(false)
{

	// initialize screen height, width
	int h, w;
	getTerminalSize(h, w);
	LOG("[Screen::Screen] (h,w)=(%d,%d)", h, w);
	// TODO: if w/h is smaller then MIN, exit?

	sc_h = h;
	sc_w = w;
	top_y = SC_PADDING_Y;
	bottom_y = sc_h-1-SC_PADDING_Y; // sc_h-1 is the bottommost line
	left_x = SC_PADDING_X;
	right_x = sc_w-1-SC_PADDING_X; // sc_w-1 is the rightmost line
	mid_x = sc_w/2;
	cal_end_y = SC_PADDING_Y+1+MIN_CAL_H;


	// initialize submodules
	calendar = std::make_shared<Calendar>(top_y+1, left_x+1,
		MIN_CAL_H,((sc_w-SC_PADDING_X*2-1)/2)-1);
	submods.push_back(calendar);
	dailytasks = std::make_shared<taskPanel>(cal_end_y+3, left_x+5,
		bottom_y-1-(cal_end_y+3), mid_x-(left_x+5));
	submods.push_back(dailytasks);

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
		refreshScr();
		rc = 0;
	}
}

void Screen::printFrame() {
	int fr_w = sc_w-2*SC_PADDING_X;
	int fr_h = sc_h-2*SC_PADDING_Y;
	std::string x_char = u8"\u2550";
	std::string y_char = u8"\u2551";
	std::string tl_char = u8"\u2554"; // top left
	std::string tr_char = u8"\u2557"; // top right
	std::string bl_char = u8"\u255A"; // bottom left
	std::string br_char = u8"\u255D"; // bottom right
	std::string mdn_char = u8"\u2566"; // v
	std::string mup_char = u8"\u2569"; // ^
	std::string mlft_char = u8"\u2563"; // <=|
	std::string mrt_char = u8"\u2560"; //    |=>


	// TOP
	for (int xsft=0; xsft<=fr_w; xsft++) {
		mvprintw(top_y, left_x+xsft, x_char.c_str());
	}
	// BOTTOM
	for (int xsft=0; xsft<=fr_w; xsft++) {
		mvprintw(bottom_y, left_x+xsft, x_char.c_str());
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
	mvprintw(SC_PADDING_Y, sc_w-SC_PADDING_X, tr_char.c_str());
	// BOTTOM-LEFT
	mvprintw(sc_h-1-SC_PADDING_Y, SC_PADDING_X, bl_char.c_str());
	// BOTTOM-RIGHT
	mvprintw(sc_h-1-SC_PADDING_Y, sc_w-SC_PADDING_X, br_char.c_str());


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
}

void Screen::printScr()
{
	printFrame();
	calendar->print();
	dailytasks->print();
}

void Screen::refreshScr()
{
	erase();
	printScr();
	refresh();
}

int Screen::handleOp(int ch) {
	if (!delegESC && isESC(ch)) {
		LOG("[Screen::handleOp] opening setting panel");
		//TODO: setting panel?
		return 0;
	} else {
		return submods[mode]->handleOp(ch);
	}
}

void Screen::handleRC(int rc)
{
	// LOG("[Screen::handleRC] rc=[%d]", rc);
	if (rc==START_DELEGESC) {
		delegESC = true;
		if (mode==0) update_dailytasks();
	}
	else if (rc==STOP_DELEGESC) delegESC = false;
	else if (rc == STOP_SC_ATPMODE) {
		// toggleAtpMode();
	} else if (rc==SC_UPDATE_DATE_2_SUBM) {
		update_dailytasks();
	}
}

void Screen::update_dailytasks() {
	std::vector<int> date = calendar->getDate();
	dailytasks->updateTasks(date[0],date[1],date[2]);
}