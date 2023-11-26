#include "../headers/tool.h"

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
	// diff foreground colors
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
	init_pair(6, COLOR_GREEN, COLOR_BLACK);
	init_pair(7, COLOR_YELLOW, COLOR_BLACK);

	// diff background colors
	init_pair(8, COLOR_BLACK, COLOR_RED);
	init_pair(9, COLOR_BLACK, COLOR_BLUE);
	init_pair(10, COLOR_BLACK, COLOR_WHITE);
	init_pair(11, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(12, COLOR_BLACK, COLOR_CYAN);
	init_pair(13, COLOR_BLACK, COLOR_GREEN);
	init_pair(14, COLOR_BLACK, COLOR_YELLOW);
	return true;
}

void mvprintwColor(int y, int x, const char* str, int color)
{
	attron(COLOR_PAIR(color));
	mvprintw(y, x, str);
	attroff(COLOR_PAIR(color));
}

void endNcurses()
{
	endwin();
}

std::tm* curTimeCompnt()
{
	auto now = std::chrono::system_clock::now();

    // Convert the system time point to a time_t object
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm structure
    std::tm* timeInfo = std::localtime(&currentTime);
	return timeInfo;
}

std::string centerText(const std::string& text, int length)
{
    int textLength = text.length();

    if (textLength > length) {
        return text;
    }

    int padding = (length - textLength) / 2;
    std::string centeredText;
    centeredText.append(padding, ' ');
    centeredText.append(text);
    centeredText.append(length - textLength - padding, ' ');
    return centeredText;
}

std::string specialWrapCenterText(const std::string& text, int length, char chr)
{
    std::string res = centerText(text, length);
    res.erase(0, 1);
    res.insert(res.begin(), chr);
    res.erase(res.length()-1);
    res.push_back(chr);
    return res;
}

std::string ljust(const std::string& text, int length, char ch)
{
    int textLength = text.length();
    if (textLength > length) {
        return text;
    }
    std::string rest = std::string(length - textLength, ch);

    return text + rest;
}

std::string rjust(const std::string& text, int length, char ch)
{
    int textLength = text.length();
    if (textLength > length) {
        return text;
    }
    std::string pre = std::string(length - textLength, ch);
    return pre + text;
}

std::string specialLJust(const std::string& text, int length, char ch)
{
    std::string ret = std::string("#");
    ret.append(ljust(text, length-2, ' '));
    ret.append("#");
    return ret;
}