all: simulate-pomo-introgression

simulate-pomo-introgression: simulate.cpp
	g++ -g -std=c++20 -O3 -Wall -o simulate-pomo-introgression simulate.cpp -lxerces-c

clean:
	$(RM) simulate-pomo-introgression