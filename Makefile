CC:=clang
SRC:=js.c
EXE:=js
RM:=rm

all: $(SRC)
	$(CC) $(SRC) -o $(EXE) -Wall -Werror -O2 -pedantic

debug: $(SRC)
	$(CC) $(SRC) -o $(EXE) -g -Wall -Werror -pedantic

clean:
	$(RM) $(EXE)

.PHONY = all clean
