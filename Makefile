.PHONY:
	all

all:
	g++ -std=c++11 -lm main.cpp -o genwav

clean:
	rm genwav
