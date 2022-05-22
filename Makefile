CC = g++

INCLUDE = ./inc
SOURCE = ./src
BUILD = ./build
BINARY = .

API = init.o game.o player.o io.o

all: $(API)
	$(CC) $(addprefix $(BUILD)/, $^) -o $(BINARY)/Hangman

$(API):
	$(CC) -c $(SOURCE)/$(basename $@).cpp -o $(BUILD)/$@ -I $(INCLUDE) -I $(SOURCE)
