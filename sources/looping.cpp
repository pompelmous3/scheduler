#include "../headers/looping.h"
#include "../headers/log.h"

void loopingMove(Screen &sc)
{
	/*
		routers: handle different forms of keys and forward them
	*/
	int ch;
	int mvx, mvy;
	while(1)
	{
		mvx = 0;
		mvy = 0;
		ch = getch();
		// Log::gI().log("[loopingMove] ch=[%d]",ch);

		if (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_LEFT || ch==KEY_RIGHT) {
			sc.handleArrow(ch);
		} else if (ch == 27) {
			sc.handleEsc();
		} else if (ch == 13) {
			sc.handleEnter();
		} else if (ch == 8) { // 127, \b ?
			sc.handleBS();
		} else {
			sc.passOp(ch);
		}
		sc.move_cs(mvx, mvy);


		sc.refreshScr();
	}
}