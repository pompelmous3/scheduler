#include "taskpanel.h"
#include "return_code.h"

taskPanel::taskPanel(int y, int x, int h, int w, std::string t)
    : idx(0), st_idx(0), displayIdx {false}, cur_y {0}, cur_m {0}, cur_d {0},
	h(h), w(w)
{
	this->y = y;
	this->x = x;
	this->title = t;
	// LOG("[taskPanel::taskPanel] initialized");
    // printMap.push_back(std::string(width, '#'));
	tasks_h = h-2;
	tasks_w = w-PFX_SPACE-CKBX_SZ;
	// LOG("[TP::TP] tasks: h[%d], w[%d]", tasks_h, tasks_w);
}

taskPanel::~taskPanel()
{
}

void taskPanel::updateTasks(int y, int m, int d)
{
	tasks.clear();

	// if y==-1, date not set, remains the same
	if (y!=-1 && (cur_y!=y || cur_m!=m || cur_d!=d)) {
		cur_y = y;
		cur_m = m;
		cur_d = d;
		idx = 0; // only reset when date changed
	}
	dbh.queryDateTasks(cur_y, cur_m, cur_d);
	std::vector<task_entry> res = dbh.getLastResults();
	for (int i=0; i<res.size(); i++) {
		tasks.push_back(res[i]);
	}

	// after updated, current date's tasks cnt might change
	idx %= tasks.size();
}

int taskPanel::get_cur_taskid()
{
	return tasks[idx].id;
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
		if (idx>=tasks.size()) {
			LOG("[TP::handleOp] isEnter, but idx>=tasks.size()");
			return rc;
		}
		tasks[idx].state = dbh.toggleState(tasks[idx].id, tasks[idx].state);
	} else if (isCtrlE(ch)) {
		LOG("[taskPanel::handleOp] is ctrl E");
		rc = TP_EDIT_REQ;
	}
    return rc;
}

void taskPanel::print()
{
	SubModule::print();
    int tx = this->x+PFX_SPACE; // 5 spaces before check box
    int ty = this->y+2; // 1st line for title

	// calculate tasks number can fit in taskPanel
	// LOG("[TP::print] tasks: h[%d], w[%d]", tasks_h, tasks_w);
	// LOG("[TP::print] tasks_h=[%d], tasks_w=[%d], idx=[%d]", tasks_h, tasks_w, idx);

	/*
	==== determine printing tasks range ====
	# the desc.size we are using for calculating lcnt should equal to 
		total size of printed "ln" later so that actual used line cnt
		could match.
	*/
	if (idx<st_idx) {
		st_idx = idx;
	} else if (idx>st_idx) { // make sure idx is in the printing range
		int rh = h-2;
		int ridx = idx+1; // checking start at (ridx-1)==(idx)
		while (true) {
			if (ridx==0) break;
			// ridx--;
			int lcnt = (tasks[ridx-1].desc.size()/tasks_w) + ((tasks[ridx-1].desc.size()%tasks_w)?1:0);
			LOG("[TP::print] checking ridx=[%d], lcnt=[%d]", ridx-1, lcnt);
			if (rh-lcnt >= 0) { // we can add this task
				rh -= lcnt;
				ridx--;
			} else {
				break;
			}
		}
		/*
		- when end, ridx should be the frontest task idx we can print
			while tasks[idx] is included
		*/
		LOG("[TP::print] ridx=[%d]", ridx);
		if (ridx > st_idx) st_idx=ridx;
	}
	
	

    // print tasks in the range determined above
	for (int i=st_idx; i<tasks.size(); i++) {
		int lcnt = (tasks[i].desc.size()/tasks_w) + ((tasks[i].desc.size()%tasks_w)?1:0);
		LOG("[TP::print] calculate h after adding [%d]: %d", i, ((ty+lcnt) - (this->y+2) + 1));
		if (((ty+lcnt) - (this->y+2)) > tasks_h) {
			LOG("[TP::print] i=[%d], breaking", i);
			break;
		}

		setTaskColor(tasks[i].priority, (idx == i) ? true:false);
		// print state symbol
		if (tasks[i].state == "TODO") {
			mvprintw(ty, tx, u8"\u25A2 ");
		} else { // DONE
			mvprintw(ty, tx, u8"\u2713 ");
		}

		// print tasks[i].desc
		std::vector<std::string> words = splitBySpace(tasks[i].desc);
		int widx = 0;
		std::string ln;
		int lnsz = 0;
		std::string token;
		while (widx<words.size()) {
			token = (widx<words.size()-1)?(words[widx]+" "):(words[widx]);

			if (lnsz+token.size()>tasks_w) {
				mvprintw(ty++, tx+CKBX_SZ, ln.c_str());
				ln.clear();
				lnsz = 0;
			}
			// append new token
			if (tasks[i].state == "TODO") {
				ln.append(token);
			} else {
				for (auto& ch:token){
					ln.push_back(ch);
					ln.append(u8"\u0336");
				}
			}
			lnsz += token.size();
			widx++;
		}
		mvprintw(ty++, tx+CKBX_SZ, ln.c_str());
        resetTaskColor(tasks[i].priority, (idx == i) ? true:false);
    }
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