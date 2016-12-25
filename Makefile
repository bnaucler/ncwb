CC = cc
TARGET = ncwb
SOURCE = ncwb.c
DESTDIR = /usr/bin
CFLAGS ?= -Wall -g
CFLAGS += -std=c99 -Wall -g
CPPFLAGS += -D_POSIX_C_SOURCE=2  # for popen/getopt
CPPFLAGS += -D_DEFAULT_SOURCE    # for usleep
LDLIBS += -lncurses

all: $(TARGET)

$(TARGET): $(SOURCE)

clean:
	rm -f $(TARGET)

install:
	cp $(TARGET) $(DESTDIR)
