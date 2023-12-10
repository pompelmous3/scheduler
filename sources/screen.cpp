#include "../headers/screen.h"
#include "../headers/log.h"

Screen::Screen(int sc_h, int sc_w)
	: sc_h {sc_h}, sc_w {sc_w}, menu(sc_h, sc_w), atp(sc_h, sc_w)
{
	getyx(stdscr, cs_y, cs_x);
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
	std::optional<std::pair<int, int>> curs; 
	if (atpMode) {
		curs = atp.print();
	} else if (menuMode) {
		menu.print();
	} else {
		for (int i=0; i<d_month_num; i++) {
			(*months[i]).printMonth();
		}
		curs = std::make_pair(cs_y, cs_x);
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
	// if (menuMode) {
	// 	curs_set(0);
	// } else {
	// 	curs_set(1);
	// }
}
void Screen::toggleAtpMode()
{
    atpMode = (atpMode + 1) % 2;
    if (atpMode) {
        // curs_set(0);
		Log::gI().log("[Screen] reassigning new atp");
		atp = addTaskPanel(sc_h, sc_w); // should auto delete previous one
    } 
	// else {
    //     curs_set(1);
    // }
}

void Screen::handleArrow(int ch)
{
	if (isMenuMode())
		menu.handleOp(ch);
	else if (isAtpMode())
		atp.handleOp(ch);
	else {
		if (ch == KEY_UP || ch == KEY_DOWN) {
			cs_y = (ch == KEY_UP) ? cs_y-1 : cs_y+1;
		} else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
			cs_x = (ch == KEY_LEFT) ? cs_x-1 : cs_x+1;
		}
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

void Screen::passOp(int ch)
{
	if (isMenuMode()) {
		menu.handleOp(ch);
	} else if (isAtpMode()) {
		atp.handleOp(ch);
	}
}
