scheduler: calendar.o log.o db.o main.o
	g++ calendar.o log.o db.o main.o -lncurses -lsqlite3 -o scheduler
	rm -f main.o log.o calendar.o db.o
calendar.o: ./sources/calendar.cpp
	g++ -c ./sources/calendar.cpp -lncurses
log.o: ./sources/log.cpp
	g++ -c ./sources/log.cpp
db.o: ./sources/db.cpp
	g++ -c ./sources/db.cpp -lsqlite3
main.o: main.cpp
	g++ -c main.cpp