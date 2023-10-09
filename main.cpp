#include <iostream>
#include <unistd.h>
#include <curses.h>
#include <chrono>
#include <ctime>
#include "./headers/log.h"
#include "./headers/calendar.h"
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

	// TODO: detect screen size to decide how many displaying months
	Month m = Month((*local_t).tm_year+1900, (*local_t).tm_mon+1, 5, 2);
	Screen sc = Screen();
	sc.addMonth(&m);
	sc.printScr();

	loopingMove(sc);
	int height, width;
	getTerminalSize(height, width);
	// cout << "h: " << height << "; w: " << width << "\n";
	getch();
	endNcurses();
	return 0;
}






