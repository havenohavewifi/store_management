CC = g++
LD = g++
CFLAGS = -Wall -c filestorage

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %cpp, %o, $(SRCS))
TARGET = filestorage

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%o: %c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGET)
