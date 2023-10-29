#ifndef LOOPING_H
#define LOOPING_H
#include "screen.h"

bool initNcurses();
bool initColors();
void endNcurses();
void loopingMove(Screen &sc); 

#endif