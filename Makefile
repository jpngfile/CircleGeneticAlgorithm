
build:
	g++ -std=c++14 -c main.cc
	g++ -std=c++14 main.o -o circle-app -lsfml-graphics -lsfml-window -lsfml-system
clean:
	rm -f main.o
