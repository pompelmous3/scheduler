#include <iostream>
#include <unistd.h>
#include <curses.h>
#include <chrono>
#include <ctime>
#include "./headers/log.h"
#include "./headers/tool.h"
#include "./headers/screen.h"

// using namespace std;

int main()
{
	if (initNcurses() == false)
	{
		endNcurses();
		exit(1);
	}

	Screen sc = Screen();
	sc.looping();
	// sc.printScr();

	// loopingMove(sc);
	getch();
	endNcurses();
	getch();
	return 0;
}






