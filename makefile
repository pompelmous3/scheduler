scheduler: calendar.o main.o
	g++ calendar.o main.o -lncurses -o scheduler
	rm -f main.o calendar.o
calendar.o: ./sources/calendar.cpp
	g++ -c ./sources/calendar.cpp -lncurses
main.o: main.cpp
	g++ -c main.cpp