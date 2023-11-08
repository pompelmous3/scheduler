#include "../headers/looping.h"
#include "../headers/log.h"

void loopingMove(Screen &sc)
{
	int ch;
	int mvx, mvy;
	while(1)
	{
		mvx = 0;
		mvy = 0;
		ch = getch();

		if (sc.passedOp(ch)==0) {
			if (ch == KEY_UP || ch == KEY_DOWN) {
				mvy = (ch == KEY_UP) ? -1 : 1;
			} else if (ch == KEY_LEFT || ch == KEY_RIGHT) {
				mvx = (ch == KEY_LEFT) ? -1 : 1;
			} else if (ch == 27) {
				sc.handleEsc();
			} else if (ch == 13) {
				sc.handleEnter();
			}
			sc.move_cs(mvx, mvy);
		}

		sc.refreshScr();
	}
}