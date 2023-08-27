#include <iostream>
#include <unistd.h>
#include <curses.h>
#include "./headers/logging.h"
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

	// printSingleMonth(5, 2024);
	Month m = Month(2024, 5, 5, 2);
	m.print();
	Screen sc = Screen();
	sc.addMonth(&m);

	loopingMove(sc);
	int height, width;
	getTerminalSize(height, width);
	// cout << "h: " << height << "; w: " << width << "\n";
	getch();
	endNcurses();
	return 0;
}






