LIBS=-lSDL2 -lSDL2_ttf -lm 
CC=mpicc

build: clean gol

gol: main.o gol.o util.o render.o
	$(CC) -O3 out/main.o out/gol.o out/util.o out/render.o $(LIBS) -o gol
main.o:
	$(CC) -c src/main.c -o out/main.o
util.o:
	$(CC) -c src/util.c -o out/util.o
gol.o:
	$(CC) -c src/gol.c -o out/gol.o
render.o:
	$(CC) -c src/render.c -o out/render.o
clean:
	rm ./out/*
