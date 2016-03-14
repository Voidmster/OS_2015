all: cat
	
clean:
	rm -f cat cat.o

cat.o: cat.c
	gcc -c cat.c

cat: cat.o
	gcc -o cat cat.o
