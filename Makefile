CC:=clang++
SRC:=js.cpp
EXE:=js
RM:=rm

all: $(SRC)
	$(CC) $(SRC) -o $(EXE) -Wall -Werror -O2 -pedantic -std=c++17

debug: $(SRC)
	$(CC) $(SRC) -o $(EXE) -g -Wall -Werror -pedantic -std=c++17

clean:
	$(RM) $(EXE)

.PHONY = all clean
