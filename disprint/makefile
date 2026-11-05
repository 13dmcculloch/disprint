CC = gcc
CFLAGS =  
LDFLAGS = -ldisprint

SRCS = disprint.c 
OBJS = $(SRCS:.c=.o)
INCS = $(SRCS:.c=.h)
TARGET = disprint

.PHONY: all 
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

.PHONY: clean
clean:
	rm $(TARGET) $(OBJS)

