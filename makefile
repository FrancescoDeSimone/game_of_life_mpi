LDFLAGS=-lSDL2 -lSDL2_ttf -lm
CFLAGS = -O3 -flto -Wall -Wextra -std=c11
CC=mpicc
OUTDIR=out
SRCDIR = src
OUTDIR = out
HEADERDIR = header
TARGET = gol

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SOURCES))

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUTDIR)/%.o: $(SRCDIR)/%.c | $(OUTDIR)
	$(CC) $(CFLAGS) -I$(HEADERDIR) -c $< -o $@

# Rule to create output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR) $(TARGET)
