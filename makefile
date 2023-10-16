# Variables
CXX := g++
CXX_LEN := c++17
CXX_MAIN := main.cpp


# Configuration
SRC_DIR := ./source
INC_DIR := ./inc
BIN_DIR := ./bin
LIB_FLAGS := # for example -L/lib/my_library -lstatic_link
DEBG_EXE := debug
PROF_EXE := profile
RELS_EXE := release


# Flags
CTRL_FLAGS := -fsyntax-only
DEBG_FLAGS := -g  # -Wfatal-errors -Wall
PROF_FLAGS := -Wfatal-errors -Wall -pg -O3
RELS_FLAGS := -w -O3


# Files
SRC := $(wildcard $(SRC_DIR)/*.cpp)
INC := $(wildcard $(INC_DIR)/*.h)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRC))


# Phony
.PHONY: clean


# ----------------------------------------------------------------
# CTRL: Perform a syntax check.
# DEBG: Create a debuggable executable.
# PROF: Generate an executable suitable for profiling.
# RELS: Build a release-ready executable.
# ----------------------------------------------------------------

CTRL: $(SRC) $(INC)
	$(CXX) $(LIB_FLAGS) $(CTRL_FLAGS) -std=$(CXX_LEN) -c $(CXX_MAIN)
	$(CXX) $(LIB_FLAGS) $(CTRL_FLAGS) -std=$(CXX_LEN) -c $(SRC)

DEBG: clean
	@for src_file in $(SRC); do \
		obj_file=$(BIN_DIR)/$$(basename $$src_file .cpp).o; \
		$(CXX) $(LIB_FLAGS) $(DEBG_FLAGS) -std=$(CXX_LEN) -c $$src_file -o $$obj_file; \
	done
	$(CXX) $(LIB_FLAGS) $(DEBG_FLAGS) -std=$(CXX_LEN) $(CXX_MAIN) $(OBJ) -o $(DEBG_EXE)

PROF: clean
	@for src_file in $(SRC); do \
		obj_file=$(BIN_DIR)/$$(basename $$src_file .cpp).o; \
		$(CXX) $(LIB_FLAGS) $(PROF_FLAGS) -std=$(CXX_LEN) -c $$src_file -o $$obj_file; \
	done
	$(CXX) $(LIB_FLAGS) $(LIB_FLAGS) $(PROF_FLAGS) -std=$(CXX_LEN) $(CXX_MAIN) $(OBJ) -o $(PROF_EXE)

RELS: clean
	@for src_file in $(SRC); do \
		obj_file=$(BIN_DIR)/$$(basename $$src_file .cpp).o; \
		$(CXX) $(LIB_FLAGS) $(RELS_FLAGS) -std=$(CXX_LEN) -c $$src_file -o $$obj_file; \
	done
	$(CXX) $(LIB_FLAGS) $(RELS_FLAGS) -std=$(CXX_LEN) $(CXX_MAIN) $(OBJ) -o $(RELS_EXE)

# private rules
clean:
	-rm -f $(BIN_DIR)/*.o
	-rm -f debug
	-rm -f profile
	-rm -f release