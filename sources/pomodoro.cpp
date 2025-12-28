#include "pomodoro.h"
#include "log.h"
#include <ncurses.h>
#include <cmath>

Pomodoro::Pomodoro(int y_pos, int x_pos, int height, int width, std::shared_ptr<DBHandler> dbh_)
    : state(IDLE), work_duration(40), short_break(5), long_break(15),
      pomodoros_until_long_break(4), completed_pomodoros(0),
      remaining_time(0), timer_running(false), should_stop_thread(false), dbh(dbh_)
{
    this->y = y_pos;
    this->x = x_pos;
    this->h = height;
    this->w = width;
    this->title = "Pomodoro Timer";
    this->hovered = false;
    
    // Initialize current date
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    current_year = 1900 + ltm->tm_year;
    current_month = 1 + ltm->tm_mon;
    current_day = ltm->tm_mday;
    
    // Start the timer thread
    timer_thread = std::thread(&Pomodoro::timer_thread_func, this);
}

Pomodoro::~Pomodoro()
{
    LOG("[~Pomodoro] destructor called");
    should_stop_thread = true;
    if (timer_thread.joinable()) {
        timer_thread.join();
    }
}

void Pomodoro::timer_thread_func()
{
    while (!should_stop_thread.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        if (timer_running.load()) {
            std::lock_guard<std::mutex> lock(timer_mutex);
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
            remaining_time -= elapsed;
            start_time = now;
            
            if (remaining_time.count() <= 0) {
                remaining_time = std::chrono::seconds(0);
                timer_running = false;
                if (state == WORK) {
                    record_session_completion();
                    state = IDLE;
                    LOG("[Pomodoro::timer_thread_func] Work session completed!");
                } else {
                    state = IDLE;
                }
            }
        }
    }
}

void Pomodoro::start_work()
{
    std::lock_guard<std::mutex> lock(timer_mutex);
    state = WORK;
    remaining_time = std::chrono::seconds(work_duration * 60);
    start_time = std::chrono::steady_clock::now();
    timer_running = true;
    LOG("[Pomodoro::start_work] Starting work session for %d minutes", work_duration);
}

void Pomodoro::start_break()
{
    std::lock_guard<std::mutex> lock(timer_mutex);
    completed_pomodoros++;
    if (completed_pomodoros % pomodoros_until_long_break == 0) {
        state = LONG_BREAK;
        remaining_time = std::chrono::seconds(long_break * 60);
    } else {
        state = SHORT_BREAK;
        remaining_time = std::chrono::seconds(short_break * 60);
    }
    start_time = std::chrono::steady_clock::now();
    timer_running = true;
    LOG("[Pomodoro::start_break] Starting break");
}

void Pomodoro::pause()
{
    LOG("[Pomodoro::pause] Pausing timer");
    if (timer_running.load() && (state == WORK || state == SHORT_BREAK || state == LONG_BREAK)) {
        std::lock_guard<std::mutex> lock(timer_mutex);
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        remaining_time -= elapsed;
        state = PAUSED;
        timer_running = false;
        LOG("[Pomodoro::pause] Timer paused");
    }
}

void Pomodoro::resume()
{
    std::lock_guard<std::mutex> lock(timer_mutex);
    if (state == PAUSED) {
        start_time = std::chrono::steady_clock::now();
        state = (completed_pomodoros % pomodoros_until_long_break == 0 && completed_pomodoros > 0) 
                ? LONG_BREAK 
                : (completed_pomodoros > 0 ? SHORT_BREAK : WORK);
        timer_running = true;
        LOG("[Pomodoro::resume] Timer resumed");
    }
}

void Pomodoro::reset()
{
    std::lock_guard<std::mutex> lock(timer_mutex);
    state = IDLE;
    timer_running = false;
    remaining_time = std::chrono::seconds(0);
    LOG("[Pomodoro::reset] Timer reset");
}



std::string Pomodoro::format_time(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", mins, secs);
    return std::string(buffer);
}

int Pomodoro::handleOp(int ch)
{   
    switch(ch) {
        case 's': // Start/Stop
        case 'S':
            if (state == IDLE) {
                start_work();
            } else if (state == WORK && !timer_running) {
                start_work();
            } else if (timer_running) {
                pause();
            } else if (state == PAUSED) {
                resume();
            }
            break;
            
        case 'r': // Reset
        case 'R':
            reset();
            break;
            
        case 'b': // Start break manually
        case 'B':
            if (state == WORK || state == IDLE) {
                start_break();
            }
            break;
    }
    
    return 0;
}

void Pomodoro::record_session_completion()
{
    dbh->recordPomodoroSession(current_year, current_month, current_day);
    LOG("[Pomodoro] Recorded session completion");
}

int Pomodoro::get_today_total_sessions()
{
    return dbh->getTotalSessionsForDay(current_year, current_month, current_day);
}

void Pomodoro::print()
{
    // Print title using base class method for consistent formatting
    SubModule::print();
    
    // Print current date
    mvprintw(y+2, x+1, "Date: %04d-%02d-%02d", current_year, current_month, current_day);
    
    // Read timer state with mutex protection
    std::lock_guard<std::mutex> lock(timer_mutex);
    
    // Print state
    const char* state_str = "";
    switch(state) {
        case IDLE:        state_str = "🟢 IDLE"; break;
        case WORK:        state_str = "⚡ WORK"; break;
        case SHORT_BREAK: state_str = "☕ SHORT BREAK"; break;
        case LONG_BREAK:  state_str = "🌟 LONG BREAK"; break;
        case PAUSED:      state_str = "⏸ PAUSED"; break;
    }
    
    mvprintw(y+3, x+1, "State: %s", state_str);
    
    // Left side: Timer display
    int left_x = x + 1;
    int right_x = x + w/2 + 2;
    
    // Print timer (left side)
    if (timer_running.load() || state == PAUSED) {
        mvprintw(y+4, left_x, "Time: %s", format_time(remaining_time.count()).c_str());
        
        // Progress bar with unicode (right side)
        // Calculate progress based on work_duration
        int total_secs = work_duration * 60;
        int elapsed_secs = total_secs - remaining_time.count();
        float progress = (total_secs > 0) ? (float)elapsed_secs / total_secs : 0.0f;
        int bar_width = 18;
        int filled = (int)(progress * bar_width);
        
        mvprintw(y+3, right_x, "Progress:");
        mvprintw(y+4, right_x, "▐");
        for (int i = 0; i < bar_width; i++) {
            if (i < filled) mvprintw(y+4, right_x+1+i, "█");
            else if (i == filled) mvprintw(y+4, right_x+1+i, "▌");
            else mvprintw(y+4, right_x+1+i, "░");
        }
        mvprintw(y+4, right_x+1+bar_width, "▌ %d%%", (int)(progress*100));
    } else {
        mvprintw(y+4, left_x, "Time: %s", format_time(work_duration * 60).c_str());
    }

    // Print today's total completed sessions
    int total_sessions = get_today_total_sessions();
    mvprintw(y+6, left_x, "Today: %d sessions (%.1fh)", total_sessions, total_sessions * 40.0 / 60.0);
    
    // Print next break info (right side)
    if (state == WORK || state == IDLE) {
        int until_long = pomodoros_until_long_break - (completed_pomodoros % pomodoros_until_long_break);
        mvprintw(y+6, right_x, "To long brk: %d", until_long);
    }
}
