#include <iostream>
#include <unistd.h>
#include <curses.h>
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

	printSingleMonth(5, 2024);

	int height, width;
	getTerminalSize(height, width);
	// cout << "h: " << height << "; w: " << width << "\n";
	getch();
	endNcurses();
	return 0;
}






