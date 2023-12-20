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

std::string getMonthStr(int month, int len)
{
	std::string mstr = months[month-1];
	// int pre_len = (len - mstr.length())/2;
	// int post_len = len - mstr.length() - pre_len;
	int post_len = len - mstr.length() - 1;
	// std::string res = std::string(pre_len, ' ') + mstr + std::string(post_len, ' ');
	std::string res = mstr + " " + std::string(post_len, '-');
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
{
	/*
		1. (x, y) is the top left point of this month
		2. (ix, iy) is the coordinate according to the initial point (x, y)
			for storing dmap for this month
	*/
	year = yr;
	month = m;
	start_weekday = getWeekDay(1, month, year);
	init_x = x;
	init_y = y;

	// get total_days, total_weeks
	int *days_months = getYearMonths(year);
	total_days = days_months[month-1];
	total_weeks = getTotalWeeks(total_days, start_weekday);

	// initialize dmap (on the base of init_x, init_y)
	dmap = new int *[total_weeks];
	for (int i=0; i<total_weeks; i++)
	{
		dmap[i] = new int[PER_LINE_IN_MONTH_LEN];
		for (int j=0; j<PER_LINE_IN_MONTH_LEN; j++)
			dmap[i][j] = -1; // init all cells to -1
	}
	int wd = start_weekday; // copy sd for iterator
	int ix = 0;
	int iy = 0;
	for (int i=1; i<=total_days; i++)
	{
		ix = wd*DATE_CELL_LEN + wd*D_DIST;
		if (i / 10 == 0) // date = 1~9
		{
			// LOG("[dmap_0]setting(%d,%d)",iy, ix+2);
			dmap[iy][ix+2] = i;
		}
		else // date >= 10
		{
			// LOG("[dmap_0]setting(%d,%d),(%d,%d)",iy, ix+1, iy, ix+2);
			dmap[iy][ix+1] = i;
			dmap[iy][ix+2] = i;
		}
		wd++;
		if (wd >= 7)
		{
			wd = 0;
			iy++;
		}
	}

	free(days_months);

	// set tasks_x, tasks_y
	tasks_x = init_x;
	tasks_y = init_y + 3 + iy + 1+1; // the 3 is the title bar
}

Month::~Month()
{
	LOG("[~Month] destructor called, [%p]", this);

	for (int i=0; i<total_weeks; i++)
		delete[] dmap[i];
	delete[] dmap;
}

int Month::getMonth()
{
	return month;
}

int Month::getYear()
{
	return year;
}

int Month::selected_day(int y, int x)
{
	int act_y = y-init_y-3; // 3 lines of headers
	int act_x = x-init_x;
	// LOG("[Month::selected_day] input=(%d,%d), act=(%d,%d)",
	// 	y, x, act_y, act_x);

	if ((act_y < 0 || act_y >= total_weeks)
		|| (act_x < 0 || act_x >= PER_LINE_IN_MONTH_LEN))
		return -1;

	return dmap[act_y][act_x];
}

void Month::printMonth()
{
	/*
		no need to refresh here, cause printMonth always invoked by
		Screen::refreshScr() => Screen::printScr()
	*/
	int y = init_y;
	int x = init_x;
	std::string mstr = getMonthStr(month, PER_LINE_IN_MONTH_LEN);
	mvprintw(y++, x, "%s", mstr.c_str());
	mvprintw(y++, x, "                           ");
	mvprintw(y++, x, "Sun Mon Tue Wed Thu Fri Sat");
	int pre_print = 0;
	for (int i=0; i<total_weeks; i++)
	{
		for (int j=0; j<PER_LINE_IN_MONTH_LEN; j++)
		{
			// no day on this pos, or this day already printed
			if (dmap[i][j] == -1 || dmap[i][j] == pre_print)
				continue;
			mvprintw(y+i, x+j, "%d", dmap[i][j]);
			pre_print = dmap[i][j];
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

