scheduler: calendar.o logging.o main.o
	g++ calendar.o logging.o main.o -lncurses -o scheduler
	rm -f main.o logging.o calendar.o
calendar.o: ./sources/calendar.cpp
	g++ -c ./sources/calendar.cpp -lncurses
logging.o: ./sources/logging.cpp
	g++ -c ./sources/logging.cpp
main.o: main.cpp
	g++ -c main.cpp