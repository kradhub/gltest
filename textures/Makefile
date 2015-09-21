BIN := gltest
SRC_FILES := main.c

CFLAGS := -g -Wall -Wextra -Wno-unused-parameter
LIBS :=

GLEW_CFLAGS := $(shell pkg-config --cflags glew)
GLEW_LIBS := $(shell pkg-config --libs glew)

GLFW_CFLAGS := $(shell pkg-config --cflags glfw3)
GLFW_LIBS := $(shell pkg-config --libs glfw3)

OBJ := $(SRC_FILES:.c=.o)
CFLAGS += $(GLEW_CFLAGS) $(GLFW_CFLAGS)
LIBS += $(GLEW_LIBS) $(GLFW_LIBS) -lm -lSOIL

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ): $(SRC_FILES)
	$(CC) -o $@ -c $< $(CFLAGS)

all: $(BIN)

.PHONY: clean

clean:
	rm *.o
	rm $(BIN)
