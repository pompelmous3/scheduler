#include "../headers/db.h"
#include "../headers/log.h"
#include <cstdlib>

// static int callback(void *data, int argc, char **argv, char **azColName){
//     LOG("[db] callback function called");
//     std::vector <std::string> res = static_cast<std::vector<std::string>>(data);
//     int i;
//     for(i=0; i<argc; i++){
//         LOG("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
//     }
//     return 0;
// }

DBHandler::DBHandler(const char *p)
{
    path = p;
    // open connection
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[DBHandler] cannot open db: %s", sqlite3_errmsg(db));
        // TODO: throw exception
    }

    // create table
    snprintf(sql, sizeof(sql), "%s",
        "CREATE TABLE If not exists tasks("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "year INT NOT NULL,"
        "month INT NOT NULL,"
        "day INT NOT NULL,"
        "start_time TEXT,"
        "state TEXT NOT NULL,"
        "priority TEXT NOT NULL,"
        "description TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    // close connection
    if (rc) {
        LOG("[DBHandler] create table err: %s", sqlite3_errmsg(db));
        // free the memory obtained from sqlite3_malloc()
        sqlite3_free(zErrMsg);
        // TODO: throw exception?
    }
    sqlite3_close(db);


    // insert test entries
    snprintf(sql, sizeof(sql), "%s", "INSERT INTO "
    "tasks (year, month, day, start_time, state, priority, description) "
    "values (2023, 9, 24, '', 'Todo', 'Urgent', 'buy eggs')");
}

DBHandler::~DBHandler()
{
    // no need to free std::string, should be taken care of automatically
    if (zErrMsg != NULL)
        free(zErrMsg);
}

std::map<int, int> DBHandler::getScheduledDays(int y, int m)
{
    std::map<int, int> res;
    sqlite3_stmt *stmt;
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[getScheduledDays] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    snprintf(sql, sizeof(sql), "SELECT day FROM tasks WHERE year=%d and "
        "month=%d GROUP BY day;", y, m);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc) {
        LOG("[getScheduledDays] prep failed: %s", sqlite3_errmsg(db));
        goto end;
    }

    // TODO: sort days in ascending order?
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int d = sqlite3_column_int(stmt, 0);
        res[d] = 1;
    }

end:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return res;
}

void DBHandler::queryDateTasks(int y, int m, int d)
{
    /*
        query all tasks that is in the y/m and day is greater than d
        Ex:
            1. y=2023, m=9, d=23
            => query all tasks from 2023/09/23 to 2023/09/30
    */
   lastResults.clear();

    sqlite3_stmt *stmt;
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[queryDateTasks] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    // compile sql statement (tasks)
    snprintf(sql, sizeof(sql), "SELECT * FROM tasks WHERE year=%d and "
        "month=%d and day=%d ORDER BY description ASC;", y, m, d);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc) {
        LOG("[queryDateTasks] prep failed: %s", sqlite3_errmsg(db));
        goto end;
    }

    // get query rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int y = sqlite3_column_int(stmt, 1);
        int m = sqlite3_column_int(stmt, 2);
        int d = sqlite3_column_int(stmt, 3);
        std::string start_time = (const char*)sqlite3_column_text(stmt, 4);
        std::string state = (const char*)sqlite3_column_text(stmt, 5);
        std::string priority = (const char*)sqlite3_column_text(stmt, 6);
        std::string desc = (const char*)sqlite3_column_text(stmt, 7);
        task_entry entry = { id, y, m, d, start_time, state, priority, desc};
        lastResults.push_back(entry);
    }

    // TODO: sort entries in lastResults according to start_time?

end:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
const std::vector <task_entry> DBHandler::getLastResults() const
{
    return (std::vector <task_entry> &) lastResults;
}

void DBHandler::insertTask(int year, int month, int day, std::string start_time,
    std::string state, std::string priority, std::string desc)
{
    sqlite3_stmt *stmt;
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[queryDateTasks] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    snprintf(sql, sizeof(sql), "INSERT INTO tasks(year, month, day, start_time"
        ", state, priority, description) values (?, ?, ?, ?, ?, ?, ?)");
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc) {
        LOG("[insertTask] prep failed: %s", sqlite3_errmsg(db));
        goto end;
    }

    sqlite3_bind_int(stmt, 1, year);
    sqlite3_bind_int(stmt, 2, month);
    sqlite3_bind_int(stmt, 3, day);
    sqlite3_bind_text(stmt, 4, start_time.c_str(), start_time.size(), NULL);
    sqlite3_bind_text(stmt, 5, state.c_str(), state.size(), NULL);
    sqlite3_bind_text(stmt, 6, priority.c_str(), priority.size(), NULL);
    sqlite3_bind_text(stmt, 7, desc.c_str(), desc.size(), NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOG("[insertTask] sqlite3_step failed: %s", sqlite3_errmsg(db));
        goto end;
    }

end:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void DBHandler::toggleState(int id, std::string cur_state)
{
    std::string set_state;
    if (cur_state == "Todo") {
        set_state = "Done";
    } else if (cur_state == "Done") {
        set_state = "Todo";
    }

    sqlite3_stmt *stmt;
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[toggleState] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    snprintf(sql, sizeof(sql), "UPDATE tasks set state = ? WHERE id = ?",
        set_state.c_str(), id);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        LOG("[toggleState] prep failed: %s", sqlite3_errmsg(db));
        goto end;
    }

    sqlite3_bind_text(stmt, 1, set_state.c_str(), set_state.size(), NULL);
    sqlite3_bind_int(stmt, 2, id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOG("[toggleState] sqlite3_step failed: %s", sqlite3_errmsg(db));
        goto end;
    }

end:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}