#include <iostream>
#include <unistd.h>
#include <curses.h>
#include <chrono>
#include <ctime>
#include "./headers/log.h"
#include "./headers/calendar.h"
#include "./headers/looping.h"
#include "./headers/tool.h"
#include "./headers/addtaskpanel.h"

//#include <windows.h>
using namespace std;

int main()
{
	if (initNcurses() == false)
	{
		endNcurses();
		exit(1);
	}

	// get current year, month
	using namespace std;
	using namespace std::chrono;
	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now);
    struct tm *local_t = localtime(&tt);

	// Get terminal size
	int height, width;
	getTerminalSize(height, width);
	LOG("[main] (height, width)=(%d,%d)", height, width);

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
	Month m = Month(cur_y, cur_m, 5, 2, height, width);
	Month n = Month(next_y, next_m, 50, 2, height, width);
	Screen sc = Screen{height, width};
	// Screen sc(height, width);
	sc.addMonth(&m);
	sc.addMonth(&n);
	sc.printScr();

	loopingMove(sc);
	getch();
	endNcurses();
	getch();
	free(local_t);
	return 0;
}






