all: main.cpp
	g++ -std=c++11 -O0 -g -o main $^ -fgnu-tm -mrtm
