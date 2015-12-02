
all: clean d1s d1p d2p test4

d1s: d1s.c
	gcc -fopenmp d1s.c -o d1s

d1p: d1p.c
	gcc -fopenmp d1p.c -o d1p

d2p: d2p.c
	gcc -fopenmp -lpthread d2p.c -o d2p

clean:
	-@rm d1s d1p d2p *.o

test4:
	./d1s < inst4.txt
	./d1p < inst4.txt
	./d2p < inst4.txt
