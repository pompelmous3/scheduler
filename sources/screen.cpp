#include "../headers/screen.h"
#include "../headers/log.h"

Screen::Screen(int sc_h, int sc_w)
	: sc_h {sc_h}, sc_w {sc_w}, menu(sc_h, sc_w), atp(sc_h, sc_w),
	mon_idx {0}, d_month_num {0}, menuMode {0}, atpMode {0}, monthMode {0}
{
	getyx(stdscr, cs_y, cs_x);
}

Screen::~Screen()
{
	LOG("[~Screen] destructor called");
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
		curs = atp.print();
	} else if (menuMode) {
		menu.print();
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
		menu.moveOption(y);
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
		LOG("[Screen] reassigning new atp");
		atp = addTaskPanel(sc_h, sc_w); // should auto delete previous one
    } 
}

void Screen::toggleMonthMode()
{
	monthMode = (monthMode + 1) % 2;
}

void Screen::handleArrow(int ch)
{
	if (isMenuMode()) {
		menu.handleOp(ch);
	} else if (isAtpMode()) {
		atp.handleOp(ch);
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
        int opt = menu.getOptionIdx();
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
		rc = atp.handleOp(KEY_ENTER);
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
		atp.handleOp(KEY_BACKSPACE);
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
		menu.handleOp(ch);
	} else if (isAtpMode()) {
		atp.handleOp(ch);
	}
}
