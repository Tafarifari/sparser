TARGET = sparser

OBJ_DIR = obj
HEADERS = $(wildcard *.h)
SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

INCLUDE = 

CC := gcc
CCFLAGS := -g -Wall

main: $(OBJ_FILES) $(HEADERS)
	$(CC) $(CCFLAGS) -o $(TARGET) $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@ $(INCLUDE)

clean :
	rm -f $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean local