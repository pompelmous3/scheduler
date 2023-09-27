#include "../headers/db.h"
#include "../headers/log.h"
#include <cstdlib>

// static int callback(void *data, int argc, char **argv, char **azColName){
//     Log::gI().log("[db] callback function called");
//     std::vector <std::string> res = static_cast<std::vector<std::string>>(data);
//     int i;
//     for(i=0; i<argc; i++){
//         Log::gI().log("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
//     }
//     return 0;
// }

DBHandler::DBHandler(const char *p)
{
    path = p;
    // open connection
    rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        Log::gI().log("[DBHandler] cannot open db: %s", sqlite3_errmsg(db));
        // TODO: throw exception
    }

    // create table
    snprintf(sql, sizeof(sql), "%s",
        "CREATE TABLE tasks("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "year INT NOT NULL,"
        "month INT NOT NULL,"
        "day INT NOT NULL,"
        "start_time TEXT,"
        "state TEXT NOT NULL,"
        "priority INT NOT NULL,"
        "description TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    // close connection
    if (rc) {
        Log::gI().log("[DBHandler] create table err: %s", sqlite3_errmsg(db));
        // free the memory obtained from sqlite3_malloc()
        sqlite3_free(zErrMsg);
        // TODO: throw exception?
    }
    sqlite3_close(db);
    Log::gI().log("[DBHandler] finished constructor");
}
DBHandler::~DBHandler()
{
    // no need to free std::string, should be taken care of automatically
    if (zErrMsg != NULL)
        free(zErrMsg);
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
        Log::gI().log("[queryDateTasks] cannot open db: %s", sqlite3_errmsg(db));
        goto end;
    }

    // compile sql statement
    snprintf(sql, sizeof(sql), "SELECT * FROM tasks WHERE year=%d and "
        "month=%d and day>=%d;", y, m, d);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc) {
        Log::gI().log("[queryDateTasks] prep failed: %s", sqlite3_errmsg(db));
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
        int priority = sqlite3_column_int(stmt, 6);
        std::string desc = (const char*)sqlite3_column_text(stmt, 7);
        Log::gI().log("[loop-rows] id=%d", id);
        task_entry entry = { id, y, m, d, start_time, state, priority, desc};
        lastResults.push_back(entry);
    }

    // TODO: sort entries in lastResults according to start_time?

end:
    sqlite3_finalize(stmt);
}
const std::vector <task_entry> DBHandler::getLastResults() const
{
    return (std::vector <task_entry> &) lastResults;
}