#ifndef DB_H
#define DB_H
#include <sqlite3.h>
#include <string>
#include <vector>

typedef struct {
    int id;
    int y;
    int m;
    int d;
    std::string start_time;
    std::string state; // Todo(X), In Progress(>), Done(O)
    std::string priority; // Urgent(red), High(blue), Normal(white)
    std::string desc;
} task_entry;

class DBHandler {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string path;
    char sql[1024];
    int last_y;
    int last_m;
    int last_d;
    std::vector <task_entry> lastResults;

public:
    DBHandler(const char *p);
    ~DBHandler();
    void queryDateTasks(int y, int m, int d);
    const std::vector <task_entry> getLastResults() const;
};
#endif