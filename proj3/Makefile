# Directories
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
TESTSRC_DIR = testsrc

# Google Test flags
GTEST_FLAGS = -L/opt/homebrew/opt/googletest/lib -lgtest -lgtest_main -pthread

# Executables
TESTS = teststrutils teststrdatasource teststrdatasink testdsv testxml testcsvbs testosm
TARGETS = $(addprefix $(BIN_DIR)/, $(TESTS))

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I./include -I/opt/homebrew/opt/googletest/include

# Default target
all: directories $(TARGETS) runtests

# Create directories if they don't exist
directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)/$(SRC_DIR)
	mkdir -p $(OBJ_DIR)/$(TESTSRC_DIR)

# Compile source files into object files
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/$(TESTSRC_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into executables
$(BIN_DIR)/teststrutils: $(OBJ_DIR)/$(SRC_DIR)/StringUtils.o $(OBJ_DIR)/$(TESTSRC_DIR)/StringUtilsTest.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS)

$(BIN_DIR)/teststrdatasource: $(OBJ_DIR)/$(SRC_DIR)/StringDataSource.o $(OBJ_DIR)/$(TESTSRC_DIR)/StringDataSourceTest.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS)

$(BIN_DIR)/teststrdatasink: $(OBJ_DIR)/$(SRC_DIR)/StringDataSink.o $(OBJ_DIR)/$(TESTSRC_DIR)/StringDataSinkTest.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS)

$(BIN_DIR)/testdsv: $(OBJ_DIR)/$(SRC_DIR)/DSVReader.o $(OBJ_DIR)/$(SRC_DIR)/DSVWriter.o $(OBJ_DIR)/$(TESTSRC_DIR)/DSVTest.o \
                    $(OBJ_DIR)/$(SRC_DIR)/StringDataSource.o $(OBJ_DIR)/$(SRC_DIR)/StringDataSink.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS)

$(BIN_DIR)/testxml: $(OBJ_DIR)/$(SRC_DIR)/XMLReader.o $(OBJ_DIR)/$(SRC_DIR)/XMLWriter.o $(OBJ_DIR)/$(TESTSRC_DIR)/XMLTest.o \
                    $(OBJ_DIR)/$(SRC_DIR)/StringDataSource.o $(OBJ_DIR)/$(SRC_DIR)/StringDataSink.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS) -lexpat

$(BIN_DIR)/testcsvbs: $(OBJ_DIR)/$(SRC_DIR)/CSVBusSystem.o $(OBJ_DIR)/$(TESTSRC_DIR)/CSVBusSystemTest.o \
                      $(OBJ_DIR)/$(SRC_DIR)/DSVReader.o $(OBJ_DIR)/$(SRC_DIR)/StringDataSource.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS)

$(BIN_DIR)/testosm: $(OBJ_DIR)/$(SRC_DIR)/OpenStreetMap.o $(OBJ_DIR)/$(TESTSRC_DIR)/OpenStreetMapTest.o \
                    $(OBJ_DIR)/$(SRC_DIR)/XMLReader.o $(OBJ_DIR)/$(SRC_DIR)/StringDataSource.o \
                    $(OBJ_DIR)/$(SRC_DIR)/CSVBusSystem.o $(OBJ_DIR)/$(SRC_DIR)/DSVReader.o
	$(CXX) $^ -o $@ $(GTEST_FLAGS) -lexpat

# Run tests in order
runtests: $(TARGETS)
	./$(BIN_DIR)/teststrutils
	./$(BIN_DIR)/teststrdatasource
	./$(BIN_DIR)/teststrdatasink
	./$(BIN_DIR)/testdsv
	./$(BIN_DIR)/testxml
	./$(BIN_DIR)/testcsvbs
	./$(BIN_DIR)/testosm

# Clean up object files and binaries
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all directories runtests clean
