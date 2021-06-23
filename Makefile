CC = gcc
CFLAGS = -g -std=c99 -Wall #-Werror
CFLAGS += -Wbad-function-cast
CFLAGS += -Wtype-limits -Wno-unused-variable
CFLAGS += -pedantic

TARGET = tp2

TEST = test_suite.h cache_test.c
INTERPRETE = src/interprete.h src/interprete.c 
BLOCK = src/block.h src/block.c
VIA = src/via.c src/via.h
CACHE = src/cache.h src/cache.c
COLA = src/cola.h src/cola.c
LINKER = -lm
VALGRIND = valgrind --leak-check=full --track-origins=yes --show-reachable=yes

all:
	$(CC) $(CFLAGS) $(CACHE) $(INTERPRETE) $(BLOCK) $(VIA) $(COLA) $(LINKER) src/main.c -o $(TARGET)

test:
	$(CC) $(CFLAGS) $(TEST) $(CACHE) $(BLOCK) $(VIA) $(LINKER) -o test

valgrind_test:
	$(VALGRIND) ./test

.PHONY : clean

clean:
	rm $(TARGET)
