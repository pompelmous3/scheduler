#include "../headers/screen.h"
#include "../headers/log.h"

Screen::Screen(int sc_h, int sc_w)
	: sc_h {sc_h}, sc_w {sc_w}, menu(sc_h, sc_w), atp(sc_h, sc_w)
{
	getyx(stdscr, cs_y, cs_x);
	Log::gI().log("[Screen] after init, cs=(%d,%d)", cs_y, cs_x);
}

Screen::~Screen()
{
	Log::gI().log("[~Screen] destructor called");
}

int Screen::getMonthsSize()
{
	return months.size();
}

void Screen::addMonth(Month *mp)
{
	months.push_back(mp); //use pointer to avoid copying a new Month instance
	d_month_num++;
}

void Screen::printScr()
{
	if (atpMode) {
		atp.print();
	} else if (menuMode) {
		menu.print();
	} else {
		for (int i=0; i<d_month_num; i++) {
			(*months[i]).printMonth();
		}
	}
	// move cursor after printMonth()s because those will move cursor too
	move(cs_y, cs_x);
}

void Screen::refreshScr()
{
	// TODO: clear current screen?
	// TODO: rewrite current screen with latest data
    // Log::gI().log("[refreshScr] atpMode=[%d], menuMode=[%d]", atpMode,
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

day *Screen::selected(int y, int x)
{
	int d;
	int yr;
	int mon;

	// TODO: more effecient way to check (instead of checking all months)
	for (int i=0; i<d_month_num; i++)
	{
		d = (*months[i]).selected_day(y, x);
		if (d != -1)
		{
			yr = (*months[i]).getYear();
			mon = (*months[i]).getMonth();
			day *res = new day{yr, mon, d};
			(*months[i]).dbh.queryDateTasks(yr, mon, d);
			return res;
		}
	}
	return nullptr;
}

void Screen::toggleMenuMode()
{
	menuMode = (menuMode + 1) % 2;
	if (menuMode) {
		curs_set(0);
	} else {
		curs_set(1);
	}
}
void Screen::toggleAtpMode()
{
    atpMode = (atpMode + 1) % 2;
    if (atpMode) {
        curs_set(0);
		Log::gI().log("[Screen] reassigning new atp");
		atp = addTaskPanel(sc_h, sc_w); // should auto delete previous one
    } else {
        curs_set(1);
    }
}

void Screen::handleEsc()
{
    if (isAtpMode())
		toggleAtpMode();
	else if (isMenuMode())
        toggleMenuMode();
    else
        toggleMenuMode();
    
}

void Screen::handleEnter()
{
    if (isMenuMode()) {
        int opt = menu.getOptionIdx();
        switch (opt)
        {
        case 0: // add a task, set screen.aptMode
            toggleMenuMode();
            toggleAtpMode();
            break;
        default:
            break;
        }
    } else if (isAtpMode()) { // 
		atp.handleOp(KEY_ENTER);
    } else { // check if date selected, update query cache
        int cur_x, cur_y;
        getyx(stdscr, cur_y, cur_x);
        day *dent = selected(cur_y, cur_x);
        if (dent) {
            Log::gI().log("[Screen::handleEnter] selected");
            free(dent);
        }
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

int Screen::passedOp(int ch)
{
	int passed = 0;

	// ESC and ENTER only handled by Screen because state may change
	if (isMenuMode() && ch != 27 && ch != 13) {
		menu.handleOp(ch);
		passed = 1;
	}
	if (isAtpMode() && ch != 27 && ch != 13) {
		atp.handleOp(ch);
		passed = 1;
	}
	return passed;
}
