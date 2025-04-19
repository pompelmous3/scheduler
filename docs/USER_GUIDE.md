# Scheduler User Guide

## Modules Overview

This section explains each module of the scheduler and the actions available to the user.

---

## 📅 Calendar Module

Displays a monthly calendar. Use it to navigate between months/days.

The Calendar module has two layers:

1. **Month View (Layer 1)**  
   - Displays multiple months side by side.  
   - Use `←` / `→` to navigate between months.  
   - Press `Enter` to select a month — the title will be highlighted to indicate selection.

2. **Day View (Layer 2)**  
   - After selecting a month, use arrow keys (`↑ ↓ ← →`) to navigate through the days of that month.  
   - The task list in the **Date Tasks** module automatically updates to reflect the selected date.  
   - Press `ESC` to return to Month View.

You can press `Enter` on a selected day to jump into the **Date Tasks** module.

### 🔧 Calendar: Available Actions

| Key         | Action                                    |
|-------------|-------------------------------------------|
| `← / →`     | In Month View: Move to previous/next month |
| `Enter`     | In Month View: Select a month and enter Day View |
|             | In Day View: Jump to manage Date Tasks |
| `↑ / ↓`     | In Day View: Move up/down within the month |
| `← / →`     | In Day View: Move left/right within the month |
| `ESC`       | In Day View: Return to Month View |

---

## 📋 Date Tasks Module

This module displays all tasks associated with the currently selected date in the Calendar.

Tasks are sorted first by state (`TODO` tasks appear before `DONE`), then by category, and finally by description.

### 🔧 Date Tasks: Available Actions

| Key        | Action                                                                 |
|------------|------------------------------------------------------------------------|
| `↑ / ↓`    | Move the selection cursor between tasks                                |
| `Enter`    | Toggle the state of the selected task between `"TODO"` and `"DONE"`   |
| `Ctrl+e`   | Edit the selected task (opens the Task Manager; navigation is disabled while editing) |
| `Ctrl+o` | Carry over a **past-dated** task to the current date |

---

## 📝 Task Manager Module

Used to add or edit tasks.

### 🔧 Task Manager: Available Actions

| Key        | Action                         |
|------------|---------------------------------|
| `↑ / ↓ / ← / →` | Switch between input fields     |
| `ESC`      | Cancel editing and go back      |
| `ENTER`    | Save task and return            |
