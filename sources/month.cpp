#include "month.h"
#include "tool.h"
#include "return_code.h"
#include <unistd.h> // for STDOUT_FILENO
#include <sys/ioctl.h> // ioctl() and TIOCGWINSZ

std::string months[12] = {"Jan", "Feb", "Mar", "Apr", "May",
						"Jun",	"Jul", "Aug", "Sep", "Oct",
						"Nov", "Dec"};
const int days_norm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// for calculate day in a week
const int century_code[4] = {6, 4, 2, 0};
const int month_code[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
std::string weekday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};


std::vector<int> getYearMonths(int year)
{
	std::vector<int> days_of_months(12, 0);
	for (int i=0; i<12; i++)
		days_of_months[i] = days_norm[i];
	if (year%4 == 0)
		days_of_months[1] += 1; // Feb
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

bool isLeapYear(int year)
{
	if (year % 4 == 0)
		return true;
	return false;
}

int getWeekDay(int d_, int m_, int yr_)
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
	int sum = d_ + month_code[m_-1] + century_code[(yr_/100)%4]\
		+ (yr_%100) + (yr_%100)/4;
	int remainder = sum%7;
	if (isLeapYear(yr_) && m_ <= 2)
		remainder--;
	return remainder;
}

std::string getMonthStrPost(int month, int len)
{
	std::string mstr = months[month-1];
	int post_len = len - (4+1 + mstr.length()+1);
	std::string res = std::string(post_len, '-');
	return res;
}



//########################################################################

// class Month methods
Month::Month(int yr_, int m_, int y_, int x_, std::shared_ptr<DBHandler> dbh_)
	: year {yr_}, month {m_}, init_x {x_}, init_y {y_},
	browsed {false}, selected {false}
{
	/*
		1. (x, y) is the top left point of this month
		2. (ix, iy) is the coordinate according to the initial point (x, y)
			for storing dmap for this month
	*/
	start_weekday = getWeekDay(1, month, year);

	// get total_days, total_weeks
	std::vector<int> days_months = getYearMonths(year);
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
	cs_day = dmap[idx.first][idx.second][0];

	this->dbh = dbh_;
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

std::vector<int> Month::getDate() {
	// if (!selected) return {0,0,0};
	return {year, month, cs_day, selected};
}

std::vector<int> Month::getPos() {
	return {init_y, init_x};
}

void Month::setBrowsed(int b)
{
	browsed = b;
}

void Month::setSelected(bool s)
{
	selected = s;
}

void Month::shiftPos(int v)
{
	init_x += (MON_WIDTH + MON_MARGIN)*v;
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
	cs_day = dmap[idx.first][idx.second][0];
}

int Month::handleOp(int ch)
/*
-- return 0 means this op handled in Month, otherwise return corresponding
 return code in return_code.h
*/
{
	int rc = 0;
	// LOG("[Month::handleOp] ch=[%d]", ch);
	if (! selected) {
		LOG("[Month::handleOp] not selected, but ch=[%d] received", ch);
		// if (ch==KEY_ENTER) setSelected(true);
		return 0;
	}

	// Following are triggered when "selected"
	if (ch==KEY_M_ESC) {
		LOG("[Month::handleOp] get KEY_M_ESC");
		LOG("[MOnth::handleOp] returning CAL_STOP_MON");
		return CAL_STOP_MON;
	}

	if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_RIGHT || ch==KEY_LEFT) {
		shiftIdx(ch);
		rc = SC_UPDATE_DATE_2_SUBM;
	} else if (isEnter(ch)) {
		// TODO: need to tell SC to change control to dateSpecificTasks
		rc = SC_SWITCH_2_DS;
		LOG("[Month::handleOp] no imp");
	}
	return rc;
}

void Month::print()
{
	/*
		no need to refresh here, cause print always invoked by
		Screen::refreshScr() => Screen::printScr()
	*/

	// TODO: make taskManager to trigger an update of scheduled days
	//		in Month?

	std::unordered_map<int, int> curTaskDays = dbh->getScheduledDays(year, month);
	// LOG("[Month::print] curTaskDays.size()=[%d]", curTaskDays.size());
	std::unordered_map<int, int>::iterator iter;

	int y = init_y;
	int x = init_x;
	char tmp[128];

	// month string
	int mon_str_sz = snprintf(tmp, sizeof(tmp), "%d %s", year, months[month-1].c_str());
	if (selected) {
		mvprintwColor(y, x, tmp, 201);
	} else if (browsed) {
		// mvprintwColor(y, x, tmp, 103);
		mvprintwColor(y, x, tmp, 12);
	} else {
		mvprintw(y, x, "%s", tmp);
	}

	// month string post part
	std::string mpost = getMonthStrPost(month, MON_WIDTH);
	mvprintw(y++, x+mon_str_sz+1, "%s", mpost.c_str());

	// headers
	mvprintw(y++, x, "                           ");
	mvprintw(y++, x, "Sun Mon Tue Wed Thu Fri Sat");

	// days
	for (auto const& row : dmap) {
		for (auto const& col : row.second) {
			if (col.second.size() == 0)
				continue;

			snprintf(tmp, sizeof(tmp), "%d", col.second[0]);
			// check if scheduled
			iter = curTaskDays.find(col.second[0]);
			if (iter != curTaskDays.end()) { // found
				// mvprintwColor(y+row.first, x+col.second[1], tmp, 101);
				mvprintwColor(y+row.first, x+col.second[1], tmp, 8);
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
		snprintf(tmp, sizeof(tmp), "%d", dmap[idx.first][idx.second][0]);
		if (found) {
			mvprintwColor(y+idx.first, x+dmap[idx.first][idx.second][1], tmp, 200);
		} else {
			mvprintwColor(y+idx.first, x+dmap[idx.first][idx.second][1], tmp, 102);
		}
	}
}