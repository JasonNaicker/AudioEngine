# ===== Compiler =====
CXX      := C:/msys64/mingw64/bin/g++.exe
CXXFLAGS := -std=c++20 -g -O0 -Wall -Wextra -Wno-unused-parameter \
            -fdiagnostics-color=always -MMD -MP -mavx
			
RELEASE_FLAGS := -std=c++20 -O2 -DNDEBUG -Wall -Wextra -Wno-unused-parameter \
                 -fdiagnostics-color=always -MMD -MP -mavx

# ===== Includes =====
INCLUDES := -I./include \
            -I./include/core \
            -I./include/io \
            -I./include/dsp \
            -I./include/network \
            -I./include/libraries \
            -IC:/msys64/mingw64/include \
            -IC:/msys64/mingw64/include/opus \
            -IC:/Code/CppPrograms/AudioStreamer/vcpkg/packages/xsimd_x64-windows/include
# ===== Libraries =====
LIB_DIRS := -LC:/msys64/mingw64/lib
LIBS     := -lmp3lame -lavcodec -lavformat -lavutil -lswresample -lopus

# ===== Directories =====
SRC_DIR    := ./src
BUILD_DIR  := ./build
TEST_DIR   := ./tests
RELEASE_DIR := ./release

# ===== Targets =====
TARGET        := $(BUILD_DIR)/AudioStreamer.exe
TEST_TARGET   := $(BUILD_DIR)/tests.exe
RELEASE_TARGET := $(RELEASE_DIR)/AudioStreamer.exe

# ===== Source files =====
SRCS      := $(filter-out $(SRC_DIR)/Main.cpp, $(shell find $(SRC_DIR) -name "*.cpp"))
OBJS      := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS      := $(OBJS:.o=.d)
MAIN_OBJ  := $(BUILD_DIR)/Main.o
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp" 2>/dev/null)

# ===== Colors =====
GREEN  := \033[0;32m
YELLOW := \033[0;33m
BLUE   := \033[0;34m
RED    := \033[0;31m
RESET  := \033[0m

# ===== Default =====
all: $(TARGET)
	@echo "$(GREEN)[DONE] Build complete: $(TARGET)$(RESET)"

# ===== Debug build =====
$(TARGET): $(OBJS) $(MAIN_OBJ)
	@echo "$(BLUE)[LD]$(RESET) $@"
	@$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN_OBJ) -o $@ $(LIB_DIRS) $(LIBS)

# ===== Release build =====
release: $(SRCS) $(SRC_DIR)/Main.cpp | $(RELEASE_DIR)
	@echo "$(YELLOW)[RELEASE] Building optimized release...$(RESET)"
	@$(CXX) $(RELEASE_FLAGS) $(INCLUDES) $(SRCS) $(SRC_DIR)/Main.cpp \
		-o $(RELEASE_TARGET) $(LIB_DIRS) $(LIBS)
	@echo "$(GREEN)[DONE] Release build: $(RELEASE_TARGET)$(RESET)"

# ===== Compile Main.cpp =====
$(MAIN_OBJ): $(SRC_DIR)/Main.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[CC]$(RESET) $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===== Compile all src =====
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[CC]$(RESET) $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===== Directories =====
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(RELEASE_DIR):
	@mkdir -p $(RELEASE_DIR)

-include $(DEPS)

# ===== Tests =====
test: $(OBJS)
	@if [ -z "$(TEST_SRCS)" ]; then \
		echo "$(RED)[TEST] No test files found in $(TEST_DIR)$(RESET)"; \
	else \
		echo "$(YELLOW)[TEST] Building tests...$(RESET)"; \
		$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRCS) $(OBJS) \
			-o $(TEST_TARGET) $(LIB_DIRS) $(LIBS) && \
		echo "$(GREEN)[TEST] Running tests...$(RESET)" && \
		$(TEST_TARGET) && \
		echo "$(GREEN)[TEST] All tests passed$(RESET)" || \
		echo "$(RED)[TEST] Tests failed$(RESET)"; \
	fi

# ===== Utility =====
run: all
	@echo "$(GREEN)[RUN] $(TARGET)$(RESET)"
	@$(TARGET)

# show all source files found
info:
	@echo "$(YELLOW)Sources:$(RESET)"
	@echo "$(SRCS)" | tr ' ' '\n'
	@echo "$(YELLOW)Objects:$(RESET)"
	@echo "$(OBJS)" | tr ' ' '\n'
	@echo "$(YELLOW)Tests:$(RESET)"
	@echo "$(TEST_SRCS)" | tr ' ' '\n'

# rebuild everything from scratch
rebuild: clean all

clean:
	@echo "$(RED)[CLEAN] Removing build artifacts...$(RESET)"
	@rm -rf $(BUILD_DIR) $(RELEASE_DIR)
	@echo "$(GREEN)[CLEAN] Done$(RESET)"

.PHONY: all clean run test release rebuild info