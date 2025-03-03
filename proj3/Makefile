
# Directories
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
TEST_SRC_DIR = testsrc
INCLUDE_DIR = include

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I$(INCLUDE_DIR) -I/opt/homebrew/opt/googletest/include
LDFLAGS = -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread -lexpat

# Executables
EXECUTABLES = $(BIN_DIR)/teststrutils \
              $(BIN_DIR)/teststrdatasource \
              $(BIN_DIR)/teststrdatasink \
              $(BIN_DIR)/testdsv \
              $(BIN_DIR)/testxml

# Default target
all: directories $(EXECUTABLES) runtests

# Create directories if they don't exist
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into executables
$(BIN_DIR)/teststrutils: $(OBJ_DIR)/StringUtils.o $(OBJ_DIR)/StringUtilsTest.o
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/teststrdatasource: $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSourceTest.o
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/teststrdatasink: $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSinkTest.o
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testdsv: $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/DSVWriter.o $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/DSVTest.o
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/testxml: $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/XMLWriter.o $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/XMLTest.o
	$(CXX) $^ -o $@ $(LDFLAGS)

# Run tests
runtests: $(EXECUTABLES)
	@for exe in $(EXECUTABLES); do ./$$exe; done

# Clean up
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)