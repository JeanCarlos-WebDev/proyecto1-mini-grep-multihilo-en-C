# CI3825 Sistemas de Operación I · Laboratorio · Abr-Jul 2026
# Makefile — NO MODIFICAR

CC     = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
TARGET = grep
SRCS   = main.c src/grep.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS) include/grep.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

test: $(TARGET)
	@bash tests/evaluar.sh

clean:
	rm -f $(TARGET)
