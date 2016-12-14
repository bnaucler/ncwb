CC = cc
TARGET = ncwb
SOURCE = ncwb.c
DESTDIR = /usr/bin
CFLAGS= -Wall -g -lncurses

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

install:
	cp $(TARGET) $(DESTDIR)
