CC=gcc
BINARY=./bin/main
INCLUDES=./includes/
SRC=./src/binarioNaTela.c ./src/file.c ./src/main.c
OPTIONS=-Wall -g -lm
TEST=./casos/1.in

all:
	@$(CC) $(SRC) -o $(BINARY) -I$(INCLUDES)
run:
	@$(BINARY)
clean:
	rm *.bin

debug:
	@$(CC) $(OPTIONS) $(SRC) -I$(INCLUDES) -o $(BINARY) 
	@valgrind --leak-check=yes $(BINARY) < $(TEST)
