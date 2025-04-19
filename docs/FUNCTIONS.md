# Function Reference – Scheduler

This document provides detailed descriptions of the core functions used in the scheduler tool.  
Each function includes its purpose, parameters, return value, and usage example if applicable.

---

## 📅 Calendar Module

### `void navigate_month(int direction)`

Navigates to the previous or next month in the calendar view.

- **Parameters**:
  - `direction` – `-1` for previous month, `1` for next month

- **Returns**: `void`

---

### `void select_month(int month_index)`

Selects a specific month and enters the day view layer.

- **Parameters**:
  - `month_index` – Index of the month in the current calendar year view (0-based)

- **Returns**: `void`

---

