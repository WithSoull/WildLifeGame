all:
	g++ -lncursesw -o start main.cpp terminal.cpp parse.cpp logic.cpp -O2 -I .