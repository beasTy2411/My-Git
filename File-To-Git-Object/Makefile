#compiler and flags
CC = gcc
DST = mygit
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lcrypto -lz

#Directories
SRC_DIR := src
OBJ_DIR := obj

#Automatically find all .c files in src
SRCS := $(wildcard $(SRC_DIR)/*.c)
#Convert .c file path to .o file paths in obj
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

#name of the executable
TARGET := $(DST)

#default target
all: $(TARGET)

#Link th final executable
$(TARGET): $(OBJS) 
	$(CC)  -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

#Compile .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
		mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)/*.o $(DST)

.PHONY: clean
