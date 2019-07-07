LIBS=-lSDL2 -lSDL2_ttf -lm 
CC=mpicc

build: clean gol

gol: main.o gol.o util.o render.o mpi_utils.o
	$(CC) -O3 -flto out/*.o $(LIBS) -o gol
main.o:
	$(CC) -c src/main.c -o out/main.o
util.o:
	$(CC) -c src/util.c -o out/util.o
gol.o:
	$(CC) -c src/gol.c -o out/gol.o
render.o:
	$(CC) -c src/render.c -o out/render.o

mpi_utils.o:
	$(CC) -c src/mpi_utils.c -o out/mpi_utils.o

clean:
	rm ./out/*
