
CXX = g++
CXXFLAGS += -std=c++17 -ggdb -Wall -Iheaders
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