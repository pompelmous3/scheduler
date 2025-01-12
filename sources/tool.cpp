#include "tool.h"

bool initNcurses()
{
    setlocale(LC_ALL, "");
	initscr();
	cbreak(); // read all input immediately rather than store in buffer
	nonl(); // type ENTER do not change new line
	noecho(); // input do not show on screen
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE); // enable special keys (Ex: arrow keys)
	start_color();
	refresh();
    curs_set(0);
    set_escdelay(30);
	if (initColors() == false)
        return false;
	return true;
}

void m_init_color(int num, int red, int green, int blue) {
    // convert normal RGB range from (0-255) to (0-1000)
    red = (red * 1000) / 255;
    green = (green * 1000) / 255;
    blue = (blue * 1000) / 255;
    init_color(num, red, green, blue);
}

bool initColors()
{
	if (has_colors() == FALSE)
    {
        printf("[initColors] Do not support colors, return..\n");
        return false;
    }
    LOG("[initColors] COLORS=[%d]", COLORS); // cap for init_color
    LOG("[initColors] COLOR_PAIRS=[%d]", COLOR_PAIRS); // cap for init_pair

    // define new colors (settings maybe cached by terminal)
    m_init_color(150, 0,100,255); // blue
    m_init_color(151, 170,190,255); // sky blue
    m_init_color(152,80,232,138); // green

    // init_pair(num, fore, back)
	// diff foreground colors
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
	init_pair(6, COLOR_GREEN, COLOR_BLACK);
	init_pair(7, COLOR_YELLOW, COLOR_BLACK);
    init_pair(8, 150, COLOR_BLACK);
    init_pair(9, 151, COLOR_BLACK);
    init_pair(10, 152, COLOR_BLACK);

	// diff background colors (start from 100)
	init_pair(100, COLOR_BLACK, COLOR_RED);
	init_pair(101, COLOR_WHITE, COLOR_BLUE);
	init_pair(102, COLOR_BLACK, COLOR_WHITE);
	init_pair(103, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(104, COLOR_BLACK, COLOR_CYAN);
	init_pair(105, COLOR_BLACK, COLOR_GREEN);
	init_pair(106, COLOR_BLACK, COLOR_YELLOW);

    // others
    init_pair(200, COLOR_WHITE, COLOR_BLUE);

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

bool isArrow(int ch) {
    return (ch==KEY_UP || ch==KEY_DOWN || ch==KEY_LEFT || ch==KEY_RIGHT);
}

bool isEnter(int ch) {
    return (ch==KEY_ENTER || ch==13);
}

bool isESC(int ch) {
    return (ch==KEY_M_ESC);
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