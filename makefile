LIBS=-lSDL2 -lSDL2_ttf -lm 
CC=mpicc
OUTDIR=out

build: clean gol

gol: $(OUTDIR)/main.o $(OUTDIR)/gol.o $(OUTDIR)/util.o $(OUTDIR)/render.o $(OUTDIR)/mpi_utils.o
	$(CC) -O3 -flto $(OUTDIR)/*.o $(LIBS) -o gol

$(OUTDIR)/main.o: src/main.c | $(OUTDIR)
	$(CC) -c src/main.c -o $@

$(OUTDIR)/util.o: src/util.c | $(OUTDIR)
	$(CC) -c src/util.c -o $@

$(OUTDIR)/gol.o: src/gol.c | $(OUTDIR)
	$(CC) -c src/gol.c -o $@

$(OUTDIR)/render.o: src/render.c | $(OUTDIR)
	$(CC) -c src/render.c -o $@

$(OUTDIR)/mpi_utils.o: src/mpi_utils.c | $(OUTDIR)
	$(CC) -c src/mpi_utils.c -o $@

# Rule to create output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/*
