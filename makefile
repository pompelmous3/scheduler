scheduler: calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o
	g++ -g calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o -lncurses -lsqlite3 -o scheduler
	rm -f calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o

calendar.o: ./sources/calendar.cpp
	g++ -g -c ./sources/calendar.cpp -lncurses
log.o: ./sources/log.cpp
	g++ -g -c ./sources/log.cpp
db.o: ./sources/db.cpp
	g++ -g -c ./sources/db.cpp -lsqlite3
tool.o: ./sources/tool.cpp
	g++ -g -c ./sources/tool.cpp
screen.o: ./sources/screen.cpp
	g++ -g -c ./sources/screen.cpp
screenobject.o: ./sources/screenobject.cpp
	g++ -g -c ./sources/screenobject.cpp
menu.o: ./sources/menu.cpp
	g++ -g -c ./sources/menu.cpp
addtaskpanel.o: ./sources/addtaskpanel.cpp
	g++ -g -c ./sources/addtaskpanel.cpp
looping.o: ./sources/looping.cpp
	g++ -g -c ./sources/looping.cpp
main.o: main.cpp
	g++ -g -c main.cpp