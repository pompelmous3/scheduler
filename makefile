scheduler: calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o
	g++ calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o -lncurses -lsqlite3 -o scheduler
	rm -f calendar.o log.o db.o tool.o screen.o screenobject.o menu.o addtaskpanel.o looping.o main.o

calendar.o: ./sources/calendar.cpp
	g++ -c ./sources/calendar.cpp -lncurses
log.o: ./sources/log.cpp
	g++ -c ./sources/log.cpp
db.o: ./sources/db.cpp
	g++ -c ./sources/db.cpp -lsqlite3
tool.o: ./sources/tool.cpp
	g++ -c ./sources/tool.cpp
screen.o: ./sources/screen.cpp
	g++ -c ./sources/screen.cpp
screenobject.o: ./sources/screenobject.cpp
	g++ -c ./sources/screenobject.cpp
menu.o: ./sources/menu.cpp
	g++ -c ./sources/menu.cpp
addtaskpanel.o: ./sources/addtaskpanel.cpp
	g++ -c ./sources/addtaskpanel.cpp
looping.o: ./sources/looping.cpp
	g++ -c ./sources/looping.cpp
main.o: main.cpp
	g++ -c main.cpp