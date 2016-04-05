all: midtermsh

clean:
	rm -f midtermsh midtermsh.o

midtermsh.o: midtermsh.cpp
	g++ -c midtermsh.cpp

midtermsh: midtermsh.o
	g++ -o midtermsh midtermsh.o
