CC = cc
TARGET = ncwb
SOURCE = ncwb.c
DESTDIR = /usr/bin
CFLAGS ?= -Wall -g
CFLAGS += -std=c99 -Wall -g
LDLIBS += -lncurses

all: $(TARGET)

$(TARGET): $(SOURCE)

clean:
	rm -f $(TARGET)

install:
	cp $(TARGET) $(DESTDIR)
