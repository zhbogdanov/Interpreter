CC=gcc
CFLAGS=-c -Wall -Werror -pedantic -o0
LDFLAGS=
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=task10
all: task11_len.c task11_calc.c
	$(CC) $(CFLAGS) -o len task11_len.c
	$(CC) $(CFLAGS) -o calc task11_calc.c
len: task11_len
	yacc task11_len.y -o task11_len.c
calc: task11_calc
	yacc task11_calc.y -o task11_calc.c
clean: 
	rm -f task11_len.c
	rm -f task11_calc.c
	rm -f len
	rm -f calc
run len:
	./len
run calc:
	./calc