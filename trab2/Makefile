CC=gcc
BINARY=./bin/main
INCLUDES=./includes/
SRC=./src/binarioNaTela.c ./src/file.c ./src/main.c
OPTIONS=-Wall -g -lm
TEST=./casos/12.in

all:
	@$(CC) $(SRC) -o $(BINARY) -I$(INCLUDES)
run:
	@$(BINARY) < $(TEST)
clean:
	rm trab.zip
	rm ./bin/*

zip:
	zip -r trab.zip ./bin $(INCLUDES) ./src Makefile

debug:
	@$(CC) $(OPTIONS) $(SRC) -I$(INCLUDES) -o $(BINARY) 
	@valgrind --leak-check=full --track-origins=yes $(BINARY) < $(TEST)
debug2:
	@$(CC) $(OPTIONS) -fsanitize=address $(SRC) -I$(INCLUDES) -o $(BINARY) 

