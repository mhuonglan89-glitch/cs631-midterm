CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -g
OBJ = ls.o cmp.o print.o
TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c ls.h print.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
