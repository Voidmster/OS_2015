all: midtermsh

clean:
	rm -f midtermsh midtermsh.o

sigusr.o: midtermsh.c
	gcc -c midtermsh.c

sigusr: midtermsh.o
	gcc -o midtermsh midtermsh.o
