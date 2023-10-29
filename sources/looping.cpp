#include "../headers/looping.h"
#include "../headers/log.h"

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
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
	init_pair(6, COLOR_GREEN, COLOR_BLACK);
	init_pair(7, COLOR_YELLOW, COLOR_BLACK);
	init_pair(8, COLOR_BLACK, COLOR_YELLOW);
	return true;
}

void endNcurses()
{
	endwin();
}

void loopingMove(Screen &sc)
{
	int ch;
	while(1)
	{
		ch = getch();
		// Log::gI().log("[loopingMove] ch=%d", ch);

		// if (sc.isAtpMode()) {
		// 	// send ch to atp
		// } else if (sc.isMenuMode()) {
		// 	// send ch to menu
		// }

		switch(ch)
		{
			case KEY_UP:
				sc.move_cs(0, -1);
				break;
			case KEY_DOWN:
				sc.move_cs(0, 1);
				break;
			case KEY_LEFT:
				sc.move_cs(-1, 0);
				break;
			case KEY_RIGHT:
				sc.move_cs(1, 0);
				break;
			case 27: // esc
				Log::gI().log("[loppingMove] get esc");
				sc.handleEsc();
				break;
			case 13: // CR (for nonl, if nl => 10)
                sc.handleEnter();
				break;
			default:
				break;
		}
		sc.refreshScr();
	}
}