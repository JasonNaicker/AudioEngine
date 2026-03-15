CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -std=c++20 -g -fdiagnostics-color=always
INCLUDES = -I./include

SRC_DIR = ./src
BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/AudioStreamer.exe

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.exe

.PHONY: all clean