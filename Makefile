#VARIABLES
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = cursive
SRC = cursive.c


#default
all: $(TARGET)


$(TARGET): $(SRC)  #target is rebuilt whenever src file changes
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)


#clean rule
clean: 
	rm -f $(TARGET)

#run rule (ensure target is upto date)
run: $(TARGET)
	./$(TARGET)

#treat as commands instead of filename
.PHONY: all clean run 

