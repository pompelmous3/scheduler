scheduler: calendar.o log.o db.o tool.o screen.o screenobject.o taskpanel.o menu.o addtaskpanel.o looping.o main.o
	g++ -std=c++17 -g calendar.o log.o db.o tool.o screen.o screenobject.o taskpanel.o menu.o addtaskpanel.o looping.o main.o -lncursesw -lsqlite3 -o scheduler
	rm -f calendar.o log.o db.o tool.o screen.o screenobject.o taskpanel.o menu.o addtaskpanel.o looping.o main.o

calendar.o: ./sources/calendar.cpp
	g++ -std=c++17 -g -c ./sources/calendar.cpp -lncursesw
log.o: ./sources/log.cpp
	g++ -std=c++17 -g -c ./sources/log.cpp
db.o: ./sources/db.cpp
	g++ -std=c++17 -g -c ./sources/db.cpp -lsqlite3
tool.o: ./sources/tool.cpp
	g++ -std=c++17 -g -c ./sources/tool.cpp
screen.o: ./sources/screen.cpp
	g++ -std=c++17 -g -c ./sources/screen.cpp
screenobject.o: ./sources/screenobject.cpp
	g++ -std=c++17 -g -c ./sources/screenobject.cpp
taskpanel.o: ./sources/taskpanel.cpp
	g++ -std=c++17 -g -c ./sources/taskpanel.cpp
menu.o: ./sources/menu.cpp
	g++ -std=c++17 -g -c ./sources/menu.cpp
addtaskpanel.o: ./sources/addtaskpanel.cpp
	g++ -std=c++17 -g -c ./sources/addtaskpanel.cpp
looping.o: ./sources/looping.cpp
	g++ -std=c++17 -g -c ./sources/looping.cpp
main.o: main.cpp
	g++ -std=c++17 -g -c main.cpp