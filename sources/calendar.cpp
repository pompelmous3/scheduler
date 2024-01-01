#include "../headers/calendar.h"
#include "../headers/log.h"
#include <iostream>
#include <curses.h>
#include <unistd.h> // for STDOUT_FILENO
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ

#define PER_LINE_IN_MONTH_LEN 27 // 7*3 + 6*1(d=1)
#define DATE_CELL_LEN 3 // Ex: Sun, Mon, ...
#define M_MARGIN 2 // margin between months
#define D_DIST 1 // distance between days in same month
std::string months[12] = {"January", "February", "March", "April", "May",
						"June",	"July", "August", "September", "October",
						"November", "December"};
const int days_norm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// for calculate day in a week
const int century_code[4] = {6, 4, 2, 0};
const int month_code[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
std::string weekday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};



int *getYearMonths(int year)
{
	int *days_of_months = (int *)malloc(sizeof(int)*12);
	for (int i=0; i<12; i++)
		days_of_months[i] = days_norm[i];
	if (year%4 == 0)
		days_of_months[1] += 1;
	return days_of_months;
}

int getTotalWeeks(int td, int sd)
{ // td: total_days; sd: start_weekday
	int tmp_rs = (td-(7-sd));
	if (tmp_rs % 7 == 0)
		return tmp_rs/7 + 1;
	return tmp_rs/7 +2;
}

void getTerminalSize(int& height, int&width)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	height = w.ws_row;
	width = w.ws_col;
}

void printYear(int year)
{
	int height, width;
	getTerminalSize(height, width);
	// TODO
}

bool isLeapYear(int year)
{
	if (year % 4 == 0)
		return true;
	return false;
}

int getWeekDay(int day, int month, int year)
{
	 /*
		==== Calculate day in a week of a date ====
		1. sum = day + month_code + century_code + last_2_digits_of_year \
				+ quotient_of_last_2_digits_of_years_divided_by_4
		2. get the remainder of the sum divided by 7
			## if leap year and date is in Jan and Feb, will be remainder-1
		==== Example ====
		=> 15 August 2050
		1. 15 + 2 + 6 + 50 + 12 = 85
		2. 85 / 7 = 12 ... 1
		=> is a Monday

		==== return value ====
		0: Sun, 1: Mon, ..., 6: Sat
	*/
	int sum = day + month_code[month-1] + century_code[(year/100)%4]\
		+ (year%100) + (year%100)/4;
	int remainder = sum%7;
	if (isLeapYear(year) && month <= 2)
		remainder--;
	return remainder;
}

std::string getMonthStrPost(int month, int len)
{
	std::string mstr = months[month-1];
	int post_len = len - mstr.length() - 1;
	std::string res = std::string(post_len, '-');
	return res;
}

int getTaskSign(std::string state)
{
	if (state == "Todo")
		return 88; // X
	else if ( state == "In Progress")
		return 62; // >
	else if (state == "Done")
		return 79; // O
	return -1;
}

void setTaskColor(std::string priority)
{
	if (priority == "Urgent")
		attron(COLOR_PAIR(1));
	else if (priority == "High")
		attron(COLOR_PAIR(2));
	else if (priority == "Normal")
		attron(COLOR_PAIR(3));
}
void resetTaskColor(std::string priority)
{
	if (priority == "Urgent")
		attroff(COLOR_PAIR(1));
	else if (priority == "High")
		attroff(COLOR_PAIR(2));
	else if (priority == "Normal")
		attroff(COLOR_PAIR(3));
}


//########################################################################

// class Month methods
Month::Month(int yr, int m, int x, int y)
	: year {yr}, month {m}, init_x {x}, init_y {y}, browsed {false}
{
	/*
		1. (x, y) is the top left point of this month
		2. (ix, iy) is the coordinate according to the initial point (x, y)
			for storing dmap for this month
	*/
	start_weekday = getWeekDay(1, month, year);

	// get total_days, total_weeks
	int *days_months = getYearMonths(year);
	total_days = days_months[month-1];
	total_weeks = getTotalWeeks(total_days, start_weekday);
	// LOG("[Month::Month] total_days=[%d], total_weeks=[%d]",
	// 	total_days, total_weeks);

	// initialize dmap (on the base of init_x, init_y)
	for (int i=0; i<total_weeks; i++)
	{ // j(wd): 0==Sun, 6==Sat
		for (int j=0; j<7; j++) {
			dmap[i][j] = std::vector<int>();
		}
	}
	int wd = start_weekday; // copy sd for iterator
	int ix = 0;
	int iy = 0;
	for (int i=1; i<=total_days; i++)
	{
		ix = wd*DATE_CELL_LEN + wd*D_DIST;
		if (i / 10 == 0) // date = 1~9
		{
			dmap[iy][wd].insert(dmap[iy][wd].end(), {i, ix+2});
		}
		else // date >= 10
		{
			dmap[iy][wd].insert(dmap[iy][wd].end(), {i, ix+1});
		}
		wd++;
		if (wd >= 7)
		{
			wd = 0;
			iy++;
		}
	}
	idx = std::make_pair(0, start_weekday);
	free(days_months);

	// set tasks_x, tasks_y
	tasks_x = init_x;
	tasks_y = init_y + 3 + iy + 1+1; // the 3 is the title bar
}

Month::~Month()
{
	LOG("[~Month] destructor called, [%p]", this);
}

int Month::getMonth()
{
	return month;
}

int Month::getYear()
{
	return year;
}

void Month::setBrowsed(int b)
{
	browsed = b;
}

void Month::setSelected(int s)
{
	selected = s;
}

void Month::shiftIdx(int ch)
{
	do {
		if (ch == KEY_UP) {
			idx.first -= 1;
		} else if (ch == KEY_DOWN) {
			idx.first += 1;
		} else if (ch == KEY_RIGHT) {
			idx.second += 1;
		} else if (ch == KEY_LEFT) {
			idx.second -= 1;
		}

		// check idx range
		if (idx.first >= total_weeks) {
			idx.first = idx.first % total_weeks;
		} else if (idx.first < 0) {
			idx.first = (total_weeks-1) - ((0-idx.first)-1);
		}
		if (idx.second >= 7) {
			idx.second = idx.second % 7;
		} else if (idx.second < 0) {
			idx.second = (7-1) - ((0-idx.second)-1);
		}
	} while (dmap[idx.first][idx.second].size() == 0);
	// size 0 means this entry wasn't initialized as a day
}

void Month::handleOp(int ch)
{
	if (! selected) {
		LOG("[Month::handleOp] not selected, but ch=[%d] received", ch);
		return;
	}

	if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_RIGHT || ch==KEY_LEFT) {
		shiftIdx(ch);
	} else if (ch == KEY_ENTER) {
		LOG("[Month::handleOp] enter pressed, TASK Mode TODO");
		dbh.queryDateTasks(year, month, dmap[idx.first][idx.second][0]);
	}
}

void Month::printMonth()
{
	/*
		no need to refresh here, cause printMonth always invoked by
		Screen::refreshScr() => Screen::printScr()
	*/

	std::map<int, int> curTaskDays = dbh.getScheduledDays(year, month);
	std::map<int, int>::iterator iter;

	int y = init_y;
	int x = init_x;
	char tmp[128];

	// month string
	sprintf(tmp, "%s", months[month-1].c_str());
	if (browsed) {
		mvprintwColor(y, x, months[month-1].c_str(), 11);
	} else {
		mvprintw(y, x, months[month-1].c_str());
	}

	// month string post part
	std::string mpost = getMonthStrPost(month, PER_LINE_IN_MONTH_LEN);
	mvprintw(y++, x+months[month-1].size()+1, "%s", mpost.c_str());

	// headers
	mvprintw(y++, x, "                           ");
	mvprintw(y++, x, "Sun Mon Tue Wed Thu Fri Sat");

	// days
	for (auto const& row : dmap) {
		// row.first == iy
		// row.second == std::map<int, std::vector<int>>
		for (auto const& col : row.second) {
			// col.first == col (wd)
			// col.second == std::vector(day, ix)
			if (col.second.size() == 0)
				continue;
			// LOG("[printMonth] [%d][%d]=(%d,%d)", row.first, col.first,
			// 	col.second[0], col.second[1]);

			sprintf(tmp, "%d", col.second[0]);
			// check if scheduled
			iter = curTaskDays.find(col.second[0]);
			if (iter != curTaskDays.end()) { // found
				mvprintwColor(y+row.first, x+col.second[1], tmp, 9);
			} else {
				mvprintw(y+row.first, x+col.second[1], "%d", col.second[0]);
			}
		}
	}
	// days cursor (overwrite)
	if (selected) {
		bool found = false;
		iter = curTaskDays.find(dmap[idx.first][idx.second][0]);
		if (iter != curTaskDays.end()) {
			found = true;
		}
		sprintf(tmp, "%d", dmap[idx.first][idx.second][0]);
		if (found) {
			mvprintwColor(y+idx.first, x+dmap[idx.first][idx.second][1], tmp, 15);
		} else {
			mvprintwColor(y+idx.first, x+dmap[idx.first][idx.second][1], tmp, 7);
		}
	}
	printTasks();
}

void Month::printTasks()
{
	std::vector <task_entry> lr = dbh.getLastResults();
	// LOG("lr.size()=%d, tasks_x=%d, tasks_y=%d",
	// 	lr.size(), tasks_x, tasks_y);
	int tx = tasks_x;
	int ty = tasks_y;
	int sign;
	// mvprintw(tasks_y, tasks_x, "aaa");

	attron(COLOR_PAIR(1));
	for (int i=0; i<lr.size(); i++) {
		sign = getTaskSign(lr[i].state);
		setTaskColor(lr[i].priority);
		// LOG("desc=%s",lr[i].desc.c_str());
		mvprintw(ty, tx, "%c %s", sign, lr[i].desc.c_str());
		resetTaskColor(lr[i].priority);
		ty++;
	}
	attroff(COLOR_PAIR(1));
}

