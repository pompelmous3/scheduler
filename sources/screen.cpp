#include "../headers/screen.h"
#include "../headers/calendar.h"
#include "../headers/log.h"

#include <chrono>
#include <ctime>

// Screen::Screen(int sc_h, int sc_w)
Screen::Screen()
	: mon_idx {0}, d_month_num {0}, menuMode {0}, atpMode {0},
	monthMode {0}
{

	// get current year, month
	using namespace std;
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);
    struct tm *local_t = localtime(&tt);

	// initialize screen height, width
	int h, w;
	getTerminalSize(h, w);
	LOG("[Screen::Screen] (h,w)=(%d,%d)", h, w);

	// Compose months
	int cur_y, cur_m;
	int next_y, next_m;
	cur_y = (*local_t).tm_year+1900;
	cur_m = (*local_t).tm_mon+1;
	if (cur_m<12) {
		next_y = cur_y;
		next_m = cur_m+1;
	} else {
		next_y = cur_y+1;
		next_m = 1;
	}
	// TODO: decide how many displaying months accordingly
	Month *m = new Month(cur_y, cur_m, 5, 2, h, w);
	Month *n = new Month(next_y, next_m, 50, 2, h, w);

	addMonth(m);
	addMonth(n);

	// initialize submodules
	sc_h = h;
	sc_w = w;
	menu = std::make_shared<Menu>(h,w);
	atp = std::make_shared<addTaskPanel>(h,w);


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
		LOG("[loopingMove] ch=[%d]",ch);

		if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_LEFT || ch==KEY_RIGHT) {
			handleArrow(ch);
		} else if (ch == 27) {
			handleEsc();
		} else if (ch == 13) {
			rc = handleEnter();
		} else if (ch == 8) { // 127, \b ?
			handleBS();
		} else {
			passOp(ch);
		}
		handleRC(rc);
		move_cs(mvx, mvy);

		refreshScr();
		rc = 0;
	}
}

int Screen::getMonthsSize()
{
	return months.size();
}

void Screen::addMonth(Month *mp)
{
	months.push_back(mp); //use pointer to avoid copying a new Month instance
	if (months.size() == 1) {
		mp->setBrowsed(1);
		mon_idx = 0;
	}
	d_month_num++;
}

void Screen::printScr()
{
	std::optional<std::pair<int, int>> curs;
	if (atpMode) {
		curs = atp->print();
	} else if (menuMode) {
		menu->print();
	} else {
		for (int i=0; i<d_month_num; i++) {
			(*months[i]).printMonth();
		}
		// curs = std::make_pair(cs_y, cs_x);
	}
	// move cursor after all printed
	if (curs) {
		move((*curs).first, (*curs).second);
		curs_set(1);
	} else {
		curs_set(0);
	}
}

void Screen::refreshScr()
{
	// TODO: clear current screen?
	// TODO: rewrite current screen with latest data
	// 	menuMode);
	erase();
	printScr();
	refresh();
}

void Screen::move_cs(int x, int y)
{
	if (menuMode) {
		menu->moveOption(y);
	} else {
		cs_x += x;
		cs_y += y;
	}
}

void Screen::shiftMonth(int v)
{
	months[mon_idx]->setBrowsed(0);
	mon_idx += v;
	if (mon_idx >= months.size()) {
		mon_idx = mon_idx % months.size();
	} else if (mon_idx < 0) {
		mon_idx = (months.size()-1) - ((0-mon_idx)-1);
	}
	months[mon_idx]->setBrowsed(1);
}

void Screen::toggleMenuMode()
{
	menuMode = (menuMode + 1) % 2;
}
void Screen::toggleAtpMode()
{
    atpMode = (atpMode + 1) % 2;
    if (atpMode) {
		// It's more reasonable to NOT recreate atp every time
		LOG("[Screen] try NOT to reassign new atp");
		// LOG("[Screen] reassigning new atp");
		// atp = std::make_shared<addTaskPanel>(sc_h, sc_w); // should auto delete previous one
    }
}

void Screen::toggleMonthMode()
{
	monthMode = (monthMode + 1) % 2;
}

void Screen::handleArrow(int ch)
{
	if (isMenuMode()) {
		menu->handleOp(ch);
	} else if (isAtpMode()) {
		atp->handleOp(ch);
	} else if (isMonthMode()) {
		months[mon_idx]->handleOp(ch);
	}
	else {
		if (ch == KEY_LEFT || ch == KEY_RIGHT) {
			int v = (ch == KEY_RIGHT)? 1 : -1;
			shiftMonth(v);
		}
	}
}

void Screen::handleEsc()
{
    if (isAtpMode()) {
		toggleAtpMode();
	} else if (isMenuMode()) {
		toggleMenuMode();
	} else if (isMonthMode()) {
		int rc = months[mon_idx]->handleOp(KEY_M_ESC);
		LOG("[Screen::handleEsc] rc=[%d]", rc);
		if (rc == 0) {
			return;
		} else if (rc == STOP_SC_MONTHMODE) {
			toggleMonthMode();
			months[mon_idx]->setSelected(0);
		}
	} else {
		toggleMenuMode();
	}
}

void Screen::handleRC(int rc)
{
	if (rc == STOP_SC_ATPMODE) {
		toggleAtpMode();
	}
}

int Screen::handleEnter()
{
	int rc = 0;
    if (isMenuMode()) {
        int opt = menu->getOptionIdx();
        switch (opt)
        {
        case 0: // add a task, set screen.aptMode
            toggleMenuMode();
            toggleAtpMode();
            break;
		case 1:
			endNcurses();
			exit(0);
        default:
            break;
        }
    } else if (isAtpMode()) { // 
		rc = atp->handleOp(KEY_ENTER);
    } else if (isMonthMode()) {
		months[mon_idx]->handleOp(KEY_ENTER);
	} else {
		toggleMonthMode(); // should be turn on
		months[mon_idx]->setSelected(1);
    }
	return rc;
}

void Screen::handleBS()
{
	if (isAtpMode()) {
		atp->handleOp(KEY_BACKSPACE);
	}
}

int Screen::isMenuMode()
{
	return menuMode;
}

int Screen::isAtpMode()
{
	return atpMode;
}

bool Screen::isMonthMode()
{
	return monthMode;
}

void Screen::passOp(int ch)
{
	if (isMenuMode()) {
		menu->handleOp(ch);
	} else if (isAtpMode()) {
		atp->handleOp(ch);
	}
}
