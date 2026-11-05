CC = gcc
CFLAGS = -fPIC -shared
LDFLAGS =

SRCS = disprint.c
OBJS = $(SRCS:.c=.o)
INCS = $(SRCS:.c=.h)
TARGET = libdisprint.so

.PHONY: all 
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS)
$(OBJS): $(SRCS) $(INCS)
	$(CC) -c -fpic $(SRCS) -o $(OBJS)

.PHONY: clean
clean:
	rm $(TARGET) $(OBJS)

