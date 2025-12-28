#ifndef POMODORO_H
#define POMODORO_H
#include "submodule.h"
#include "tool.h"
#include "db.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <ctime>

enum PomodoroState {
    IDLE,
    WORK,
    SHORT_BREAK,
    LONG_BREAK,
    PAUSED
};

class Pomodoro : public SubModule
{
private:
    size_t h, w;
    PomodoroState state;
    int work_duration;      // minutes
    int short_break;        // minutes
    int long_break;         // minutes
    int pomodoros_until_long_break;
    int completed_pomodoros;
    
    std::chrono::steady_clock::time_point start_time;
    std::chrono::seconds remaining_time;
    std::atomic<bool> timer_running;
    std::atomic<bool> should_stop_thread;
    
    std::shared_ptr<DBHandler> dbh;
    int current_year, current_month, current_day;
    
    std::thread timer_thread;
    std::mutex timer_mutex;
    
    void timer_thread_func();

public:
    Pomodoro(int y_pos, int x_pos, int height, int width, std::shared_ptr<DBHandler> dbh);
    ~Pomodoro() override;
    int handleOp(int ch) override;
    void print() override;
    
    void start_work();
    void start_break();
    void pause();
    void resume();
    void reset();
    std::string format_time(int seconds);
    bool is_timer_running() const { return timer_running.load(); }
    void record_session_completion();
    int get_today_total_sessions();
    void start_timer_thread();
    void stop_timer_thread();
};

#endif
