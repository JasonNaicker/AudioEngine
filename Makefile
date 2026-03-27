# ===== Compiler =====
CXX      := C:/msys64/mingw64/bin/g++.exe
CXXFLAGS := -std=c++20 -g -O0 -Wall -Wextra -fdiagnostics-color=always -MMD -MP

# ===== Includes =====
INCLUDES := -I./include -I./include/configurations -I./include/libraries \
            -IC:/msys64/mingw64/include -IC:/msys64/mingw64/include/opus

# ===== Libraries =====
LIB_DIRS := -LC:/msys64/mingw64/lib
LIBS     := -lmp3lame -lavcodec -lavformat -lavutil -lswresample -lopus

# ===== Directories =====
SRC_DIR   := ./src
BUILD_DIR := ./build

# ===== Target =====
TARGET := $(BUILD_DIR)/AudioStreamer.exe

# ===== Source / Object / Dep files =====
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# ===== Build rules =====
all: $(TARGET)

$(TARGET): $(OBJS)
    @echo "[LD] $@"
    @$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIB_DIRS) $(LIBS)

$(BUILD_DIR):
    @mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
    @echo "[CC] $<"
    @$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

# ===== Utility =====
run: all
    @$(TARGET)

clean:
    rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.exe $(BUILD_DIR)/*.d

.PHONY: all clean run