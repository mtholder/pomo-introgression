all: simulate-pomo-introgression

cli.o: cli.h cli.cpp
	g++ -g -std=c++20 -O3 -Wall -o cli.o -c cli.cpp -lboost_program_options -lg3log

simulate-pomo-introgression: simulate.cpp cli.o
	g++ -g -std=c++20 -O3 -Wall -o simulate-pomo-introgression simulate.cpp cli.o -lboost_program_options -lg3log

clean:
	$(RM) simulate-pomo-introgression