CC=gcc
CFLAGS=-Wall
EXE_DETECT=server

all: $(EXE_DETECT)

$(EXE_DETECT): server.c support.h support.c
	$(CC) $(CFLAGS) -o $(EXE_DETECT) server.c support.h support.c

clean:
	rm -f *.o $(EXE_DETECT)
