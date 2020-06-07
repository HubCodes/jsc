CC:=clang
SRC:=js.c lib.c lex.c
EXE:=js
RM:=rm
CFLAGS:=-Wall -Werror -O2 -pedantic -std=c11

.PHONY = all clean default

all: $(SRC)
	$(CC) $(SRC) -o $(EXE) $(CFLAGS)

debug: $(SRC)
	$(CC) $(SRC) -o $(EXE) -g $(CFLAGS)

default: all

clean:
	$(RM) $(EXE)
