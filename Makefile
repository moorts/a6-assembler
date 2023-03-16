SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INSTALL_DIR := /usr/local/bin

EXE := $(BIN_DIR)/a6
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall
LDFLAGS  := -Llib
LDLIBS   := -lm

.PHONY: all install clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

install:
	install -m 557 $(BIN_DIR)/a6 $(INSTALL_DIR)

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
