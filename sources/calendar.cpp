#include "../headers/calendar.h"
#include "../headers/logging.h"
#include <iostream>
#include <curses.h>
#include <unistd.h> // for STDOUT_FILENO
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ

#define PER_LINE_IN_MONTH_LEN 27
std::string months[12] = {"January", "February", "March", "April", "May",
						"June",	"July", "August", "September", "October",
						"November", "December"};
const int days_norm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// for calculate day in a week
const int century_code[4] = {6, 4, 2, 0};
const int month_code[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
std::string weekday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


bool initNcurses()
{
	initscr();
	cbreak(); // read all input immediately rather than store in buffer
	nonl(); // type ENTER do not change new line
	noecho(); // input do not show on screen
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE); // enable special keys (Ex: arrow keys)
	start_color();
	refresh();

	if (initColors() == false)
        return false;
	return true;
}

bool initColors()
{
	if (has_colors() == FALSE)
    {
        printf("[initColors] Do not support colors, return..\n");
        return false;
    }
	return true;
}

void endNcurses()
{
	endwin();
}

void loopingMove(Screen &sc)
{
	int ch;
	int cur_x;
	int cur_y;
	Logging &logger = Logging::getInstance();
	logger.log("[loopingMove] size of screen months=[%d]", sc.getMonthsSize());
	while(1)
	{
		getyx(stdscr, cur_y, cur_x);
		logger.log("[loopingMove] cur_x=[%d], cur_y=[%d]", cur_x, cur_y);
		ch = getch();
		switch(ch)
		{
			case KEY_UP:
				move(cur_y-1, cur_x);
				break;
			case KEY_DOWN:
				move(cur_y+1, cur_x);
				break;
			case KEY_LEFT:
				move(cur_y, cur_x-1);
				break;
			case KEY_RIGHT:
				move(cur_y, cur_x+1);
				break;
			case 27: // esc
				logger.log("[loppingMove] get esc key, break loppingMove");
				return;
			default:
				break;
		}
		refresh();
	}
}

int *getYearMonths(int year)
{
	int *days_of_months = (int *)malloc(sizeof(int)*12);
	for (int i=0; i<12; i++)
		days_of_months[i] = days_norm[i];
	if (year%4 == 0)
		days_of_months[1] += 1;
	return days_of_months;
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

void printSingleMonth(int month, int year)
{
	/*
		print single month of a year
			not considering terminal width and height in single month print
			(presume the width and height should be enought for single month)
	*/
	int weekday = getWeekDay(1, month, year); // initial weekday
	int *days_months = getYearMonths(year);
	int days = days_months[month-1];
	std::string month_str = getMonthStr(month, PER_LINE_IN_MONTH_LEN);
    int initial_y = 2;
    int initial_x = 5;
	int y = initial_y;
	int x = initial_x;
	
	mvprintw(y++, x, "%s", month_str.c_str());
	mvprintw(y++, x, "                           ");
	mvprintw(y++, x, "Sun Mon Tue Wed Thu Fri Sat");
	// move x to the start weekday
	x = initial_x + weekday*4;
	for (int i=1; i<=days; i++) // i=date in the month
    {
        if (i / 10 == 0)
            x += 2;
        else
            x += 1;
        mvprintw(y, x, "%d", i);

        // move to next position
        weekday++;
        if (weekday >= 7)
        {
            weekday = 0;
            y++;
        }
        x = initial_x + weekday*4;
    }
	refresh();
	free(days_months);
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



//########################################################################

// class Month methods
Month::Month(int yr, int m, int x, int y)
{
	year = yr;
	month = m;
	start_weekday = getWeekDay(1, month, year);
	init_x = x;
	init_y = y;
	// TODO: initialize end_x, end_y
}

Month::~Month()
{
	Logging &logger = Logging::getInstance();
	logger.log("[~Month] destructor called, [%p]", this);
}

void Month::print()
{
	Logging &logger = Logging::getInstance();
	int weekday = start_weekday;
	int *days_months = getYearMonths(year);
	int days = days_months[month-1];
	std::string month_str = getMonthStr(month, PER_LINE_IN_MONTH_LEN);
	int y = init_y;
	int x = init_x;
	mvprintw(y++, x, "%s", month_str.c_str());
	mvprintw(y++, x, "                           ");
	mvprintw(y++, x, "Sun Mon Tue Wed Thu Fri Sat");
	// move x to the start weekday
	x = init_x + weekday*4;
	for (int i=1; i<=days; i++) // i=date in the month
    {
        if (i / 10 == 0)
            x += 2;
        else
            x += 1;
        mvprintw(y, x, "%d", i);

        // move to next position
        weekday++;
        if (weekday >= 7)
        {
            weekday = 0;
            y++;
        }
        x = init_x + weekday*4;
    }
	refresh();
	free(days_months);
}


// class Screen methods

Screen::Screen()
{

}
Screen::~Screen()
{
	Logging &logger = Logging::getInstance();
	logger.log("[~Screen] destructor called");
}
int Screen::getMonthsSize()
{
	return months.size();
}
void Screen::addMonth(Month *mp)
{
	months.push_back(mp); //use pointer to avoid copying a new Month instance
}
