#include "taskpanel.h"
#include "return_code.h"

taskPanel::taskPanel(int y, int x, int h, int w, std::string t)
    : idx {0}, displayIdx {false}, cur_y {0}, cur_m {0}, cur_d {0}
{
	this->y = y;
	this->x = x;
	this->title = t;
	// LOG("[taskPanel::taskPanel] initialized");
    // printMap.push_back(std::string(width, '#'));
}

taskPanel::~taskPanel()
{
}

void taskPanel::updateTasks(int y, int m, int d)
{
	tasks.clear();
	if (!(cur_y==y && cur_m==m && cur_d==d)) {
		idx = 0; // only reset when date changed
	}
	cur_y = y;
	cur_m = m;
	cur_d = d;
	dbh.queryDateTasks(y, m, d);
	std::vector<task_entry> res = dbh.getLastResults();
	for (int i=0; i<res.size(); i++) {
		tasks.push_back(res[i]);
	}
}

int taskPanel::setDisplayIdx(bool v)
{
	if (v && tasks.size() == 0) {
		return TASKPANEL_ZERO_LEN;
	}
	displayIdx = v;
	return 0;
}

int taskPanel::handleOp(int ch)
{
	int rc = 0;
	if (ch==KEY_UP) {
		idx -= 1;
		if (idx < 0) {
			idx = (tasks.size()-1) - (0-idx-1);
		}
	} else if (ch==KEY_DOWN) {
		idx += 1;
		if (idx >= tasks.size()) {
			idx = idx - tasks.size();
		}
	} else if (isEnter(ch)) {
		tasks[idx].state = dbh.toggleState(tasks[idx].id, tasks[idx].state);
	}
    return rc;
}

void taskPanel::print()
{
	SubModule::print();
    int tx = this->x+5;
    int ty = this->y+2; // 1st line for title

    for (int i=0; i<tasks.size(); i++) {
        std::string taskstr = getTaskStr(tasks[i].state, tasks[i].desc);
        setTaskColor(tasks[i].priority, (idx == i) ? true:false);
        mvprintw(ty, tx, "%s", taskstr.c_str());
        resetTaskColor(tasks[i].priority, (idx == i) ? true:false);
        ty++;
    }
}


std::string taskPanel::getTaskStr(std::string state, std::string task)
{
	std::string result;

	if (state == "TODO") { // ▢ u8"\u25A2"
		result.append(u8"\u25A2 ");
		// result.append(u8"\u2B1C ");
		// result.append(u8"\u25FB ");
		result.append(task);
	} else if (state == "DONE") { // ✓ u8"\u2713"
		result.append(u8"\u2713 ");
		// result.append(u8"\u2705 ");
		// result.append(u8"\u2611 ");
		for (auto ch : task) {
			result.push_back(ch);
			result.append(u8"\u0336");
		}
	}

	return result;
}

void taskPanel::setTaskColor(std::string priority, bool selected)
{
	// LOG("[taskPanel::setTaskColor] selected=[%d], displayIdx=[%d]", selected, displayIdx);
	if (selected && displayIdx) {
		if (priority == "NOW!")
			attron(COLOR_PAIR(100));
		else if (priority == "Soon")
			attron(COLOR_PAIR(101));
		else if (priority == "Don't care")
			attron(COLOR_PAIR(102));
	} else {
		if (priority == "NOW!")
			attron(COLOR_PAIR(1));
		else if (priority == "Soon")
			attron(COLOR_PAIR(2));
		else if (priority == "Don't care")
			attron(COLOR_PAIR(3));
	}
}

void taskPanel::resetTaskColor(std::string priority, bool selected)
{
	if (selected && displayIdx) {
		if (priority == "NOW!")
			attroff(COLOR_PAIR(100));
		else if (priority == "Soon")
			attroff(COLOR_PAIR(101));
		else if (priority == "Don't care")
			attroff(COLOR_PAIR(102));
	} else {
		if (priority == "NOW!")
			attroff(COLOR_PAIR(1));
		else if (priority == "Soon")
			attroff(COLOR_PAIR(2));
		else if (priority == "Don't care")
			attroff(COLOR_PAIR(3));
	}
}