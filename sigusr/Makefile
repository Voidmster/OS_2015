all: sigusr
	
clean:
	rm -f sigusr sigusr.o

sigusr.o: sigusr.c
	gcc -c sigusr.c

sigusr: sigusr.o
	gcc -o sigusr sigusr.o
