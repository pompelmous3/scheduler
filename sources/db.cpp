#include "db.h"
#include "log.h"
#include "tool.h"
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

    // create table "tasks"
    snprintf(sql, sizeof(sql), "%s",
        "CREATE TABLE If not exists tasks("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "year INT,"
        "month INT,"
        "day INT,"
        "start_time TEXT,"
        "last_time TEXT,"
        "repeat TEXT,"
        "category TEXT,"
        "priority TEXT NOT NULL,"
        "state TEXT NOT NULL,"
        "description TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    // close connection
    if (rc) {
        LOG("[DBHandler] create table tasks err: %s", sqlite3_errmsg(db));
        // free the memory obtained from sqlite3_malloc()
        sqlite3_free(zErrMsg);
        // TODO: throw exception?
    }

    // create table "task_cats"
    snprintf(sql, sizeof(sql), "%s",
        "CREATE TABLE if not exists task_cats("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc) {
        LOG("[DBHandler] create table task_cats err: %s", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
    }

    // create table "exp_cats"
    snprintf(sql, sizeof(sql), "%s",
        "CREATE TABLE if not exists exp_cats("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc) {
        LOG("[DBHandler] create table exp_cats err: %s", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);


    // insert test entries
    snprintf(sql, sizeof(sql), "%s", "INSERT INTO "
    "tasks (year, month, day, start_time, last_time, state, priority, description) "
    "values (2023, 9, 24, '', 'Todo', 'Urgent', 'buy eggs')");
}

DBHandler::~DBHandler()
{
    // no need to free std::string, should be taken care of automatically
    if (zErrMsg != NULL)
        free(zErrMsg);
}

std::unordered_map<int, int> DBHandler::getScheduledDays(int y, int m)
{
    std::unordered_map<int, int> res;
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
        std::string last_time = (const char*)sqlite3_column_text(stmt, 5);
        std::string repeat = (const char*)sqlite3_column_text(stmt, 6);
        std::string cat = (const char*)sqlite3_column_text(stmt, 7);
        std::string priority = (const char*)sqlite3_column_text(stmt, 8);
        std::string state = (const char*)sqlite3_column_text(stmt, 9);
        std::string desc = (const char*)sqlite3_column_text(stmt, 10);
        task_entry entry = {id,y,m,d,start_time,last_time,repeat,cat,priority,state,desc};
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

void DBHandler::insertTask(std::string year, std::string month,
    std::string day, std::string start_time, std::string last_time,
    std::string repeat, std::string cat, std::string priority,
    std::string state, std::string desc)
{
    sqlite3_stmt *stmt;
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        LOG("[queryDateTasks] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    snprintf(sql, sizeof(sql), "INSERT INTO tasks(year,month,day,start_time,"
        "last_time,repeat,category,priority,state,description)"
        "values (?,?,?,?,?,?,?,?,?,?)");
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc) {
        LOG("[insertTask] prep failed: %s", sqlite3_errmsg(db));
        goto end;
    }

    if (year.empty()) sqlite3_bind_null(stmt, 1);
    else sqlite3_bind_int(stmt, 1, std::stoi(year));
    if (month.empty()) sqlite3_bind_null(stmt, 2);
    else sqlite3_bind_int(stmt, 2, std::stoi(month));
    if (day.empty()) sqlite3_bind_null(stmt, 3);
    else sqlite3_bind_int(stmt, 3, std::stoi(day));
    sqlite3_bind_text(stmt, 4, start_time.c_str(), start_time.size(), NULL);
    sqlite3_bind_text(stmt, 5, last_time.c_str(), last_time.size(), NULL);
    sqlite3_bind_text(stmt, 6, repeat.c_str(), repeat.size(), NULL);
    sqlite3_bind_text(stmt, 7, cat.c_str(), cat.size(), NULL);
    sqlite3_bind_text(stmt, 8, priority.c_str(), priority.size(), NULL);
    sqlite3_bind_text(stmt, 9, state.c_str(), state.size(), NULL);
    sqlite3_bind_text(stmt, 10, desc.c_str(), desc.size(), NULL);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOG("[insertTask] sqlite3_step failed: %s", sqlite3_errmsg(db));
        goto end;
    }

end:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

std::string DBHandler::toggleState(int id, std::string cur_state)
{
    LOG("[DBH::toggleState] id=[%d], cur_state=[%s]", id, cur_state.c_str());
    std::string set_state;
    if (cur_state == "TODO") {
        set_state = "DONE";
    } else if (cur_state == "DONE") {
        set_state = "TODO";
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
    return set_state;
}