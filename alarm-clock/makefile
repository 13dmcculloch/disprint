CC = gcc
CFLAGS =  
LDFLAGS = -L/usr/local/lib -ldisprint

SRCS = alarm-clock.c
OBJS = $(SRCS:.c=.o)
INCS = $(SRCS:.c=.h)
TARGET = alarm-clock

.PHONY: all 
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -L/usr/local/lib $(CFLAGS) -o $(TARGET) $(SRCS) -ldisprint

.PHONY: clean
clean:
	rm $(TARGET)

