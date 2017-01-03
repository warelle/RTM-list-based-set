all: main.cpp
	g++ -std=c++11 -O0 -g -o main $^ -fgnu-tm -mrtm

l2: main.cpp
	g++ -std=c++11 -O0 -g -DL2 -o main $^ -fgnu-tm -mrtm

clean:
	rm main
