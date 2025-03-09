#ifndef DB_H
#define DB_H
// #include "tool.h"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <unordered_map>

typedef struct {
    int id;
    int y;
    int m;
    int d;
    std::string start_time;
    std::string last_time;
    std::string repeat;
    std::string cat;
    std::string priority; // Urgent(red), High(blue), Normal(white)
    std::string state; // Todo(▢), Done(✓)
    std::string desc;
} task_entry;

typedef struct {
    int id;
    std::string cname;
    int active;
} task_cat;

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
    std::vector<task_cat> cats;
    std::string activeCats; // ('cat1', 'cat2', 'cat3', ...)

    void initTables();
    void syncCatsFromDB();
public:
    DBHandler(const char *p);
    ~DBHandler();
    std::unordered_map<int, int> getScheduledDays(int, int);
    void queryDateTasks(int, int, int);
    const std::vector <task_entry> getLastResults() const;
    task_entry getTask(int tid);
    void insertTask(std::string year, std::string month,
        std::string day, std::string start_time, std::string last_time,
        std::string repeat, std::string cat, std::string priority,
        std::string state, std::string desc);
    void updateTask(std::string year, std::string month,
        std::string day, std::string start_time, std::string last_time,
        std::string repeat, std::string cat, std::string priority,
        std::string state, std::string desc, int tid);
    std::string toggleState(int id, std::string cur_state);

    // update cats, and return a copy
    std::vector<task_cat> queryTaskCats();
    void insertCat(std::string ncat);
    void removeCat(std::string ncat);
    void updateCat(std::string ncat, int active);
};
#endif