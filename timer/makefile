CC = c89
CFLAGS =
LDFLAGS = -L/usr/local/lib -ldisprint

SRCS = timer.c
OBJS = $(SRCS:.c=.o)
INCS = $(SRCS:.c=.h)
TARGET = timer

.PHONY: all 
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -L/usr/local/lib $(CFLAGS) -o $(TARGET) $(SRCS) -ldisprint

.PHONY: clean
clean:
	rm $(TARGET)

