#include "../headers/taskpanel.h"

taskPanel::taskPanel(int y, int x, int h, int w)
    : idx {0}, displayIdx {false}, init_x(x), init_y(y),
	cur_y {0}, cur_m {0}, cur_d {0}
{
	LOG("[taskPanel::taskPanel] initialized");
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
	if (v == true) {
		if (tasks.size() == 0) {
			return TASKPANEL_ZERO_LEN;
		}
	}
	displayIdx = v;
	return 0;
}

int taskPanel::handleOp(int ch)
{
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
	} else if (ch==KEY_ENTER) {
		dbh.toggleState(tasks[idx].id, tasks[idx].state);
	}
    return 0;
}

void taskPanel::print()
{
    int tx = init_x;
    int ty = init_y;

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

	if (state == "Todo") { // ▢ u8"\u25A2"
		result.append(u8"\u25A2 ");
		result.append(task);
	} else if (state == "Done") { // ✓ u8"\u2713"
		result.append(u8"\u2713 ");
		for (auto ch : task) {
			result.push_back(ch);
			result.append(u8"\u0336");
		}
	}

	return result;
}

void taskPanel::setTaskColor(std::string priority, bool selected)
{
	if (selected && displayIdx) {
		if (priority == "Urgent")
			attron(COLOR_PAIR(8));
		else if (priority == "High")
			attron(COLOR_PAIR(9));
		else if (priority == "Normal")
			attron(COLOR_PAIR(10));
	} else {
		if (priority == "Urgent")
			attron(COLOR_PAIR(1));
		else if (priority == "High")
			attron(COLOR_PAIR(2));
		else if (priority == "Normal")
			attron(COLOR_PAIR(3));
	}
}

void taskPanel::resetTaskColor(std::string priority, bool selected)
{
	if (selected && displayIdx) {
		if (priority == "Urgent")
			attroff(COLOR_PAIR(8));
		else if (priority == "High")
			attroff(COLOR_PAIR(9));
		else if (priority == "Normal")
			attroff(COLOR_PAIR(10));
	} else {
		if (priority == "Urgent")
			attroff(COLOR_PAIR(1));
		else if (priority == "High")
			attroff(COLOR_PAIR(2));
		else if (priority == "Normal")
			attroff(COLOR_PAIR(3));
	}
}