
CXX = g++
# CXXFLAGS += -std=c++17 -ggdb -Wall -Iheaders
CXXFLAGS += -std=c++17 -ggdb -Wall -Iheaders
CXXFLAGS += -Wextra -Wpedantic -Wformat=2
CXXFLAGS += -Wformat-security -Wformat-nonliteral
CXXFLAGS += -Wdeprecated -Wuninitialized -Wmaybe-uninitialized
CXXFLAGS += -Wshadow -Wdelete-non-virtual-dtor -Werror=return-type
CXXFLAGS += -Werror=unused-variable -Werror=uninitialized
CXXFLAGS += -Werror=delete-non-virtual-dtor
CXXFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2
CXXFLAGS += -O2 -fsanitize=undefined,address

LDFLAGS += -lncursesw -lsqlite3
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