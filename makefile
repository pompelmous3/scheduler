
CXX = g++
# Base flags
CXXFLAGS += -std=c++17 -Wall -Iheaders
CXXFLAGS += -Wextra -Wpedantic -Wformat=2
CXXFLAGS += -Wformat-security -Wformat-nonliteral
CXXFLAGS += -Wdeprecated -Wuninitialized
CXXFLAGS += -Wshadow -Wdelete-non-virtual-dtor -Werror=return-type
CXXFLAGS += -Werror=unused-variable -Werror=uninitialized
CXXFLAGS += -Werror=delete-non-virtual-dtor

# Debug/Release modes (comment out one)
# Debug: with sanitizers (slower compilation)
# CXXFLAGS += -g -O0 -fsanitize=undefined,address -fstack-protector-strong
# Release: optimized build (faster compilation)
CXXFLAGS += -O2

LDFLAGS = -lncursesw -lsqlite3
# Uncomment if using sanitizers
# LDFLAGS += -fsanitize=undefined,address
SOURCES = \
	./sources/calendar.cpp \
	./sources/month.cpp \
	./sources/log.cpp \
	./sources/db.cpp \
	./sources/tool.cpp \
    ./sources/screen.cpp \
	./sources/screenobject.cpp \
	./sources/taskpanel.cpp \
    ./sources/menu.cpp \
	./sources/taskmanager.cpp \
	./sources/expensemanager.cpp \
	./sources/displayfield.cpp \
	./sources/inputfield.cpp \
	./sources/categorymanager.cpp \
	./sources/submodule.cpp \
	./sources/pomodoro.cpp \
	./main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = scheduler

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)