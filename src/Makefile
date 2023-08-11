all: simulate-pomo-introgression

cli.o: cli.h cli.cpp
	g++ -g -std=c++20 -O3 -Wall -o cli.o -c cli.cpp -lboost_program_options -lg3log

simulate_cli.o: simulate_cli.h simulate_cli.cpp
	g++ -g -std=c++20 -O3 -Wall -o simulate_cli.o -c simulate_cli.cpp -lboost_program_options -lg3log

simulate-pomo-introgression: simulate_cli.h simulate.cpp cli.o simulate_cli.o
	g++ -g -std=c++20 -O3 -Wall -o simulate-pomo-introgression simulate.cpp simulate_cli.o cli.o -lboost_program_options -lg3log

clean:
	$(RM) simulate-pomo-introgression
	$(RM) cli.o simulate_cli.o