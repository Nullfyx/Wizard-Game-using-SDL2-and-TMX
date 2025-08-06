# ===================================================================
# Professional Makefile for Wizard Game
# Author: Taranveer "Nullfyx" Singh & Plexy
# ===================================================================

# --- Compiler and Flags ---
CXX = g++
# -std=c++17: Use modern C++
# -Wall -Wextra: Show all important warnings
# -MMD -MP: Generate dependency files for automatic header tracking
CXXFLAGS = -std=c++17 -Wall -Wextra -MMD -MP

# --- Libraries ---
# Linker flags to include all necessary libraries
LDFLAGS = -lSDL2 -lSDL2_image -lxml2 -ltmx -lz

# --- Project Structure ---
# The name of your final game executable
TARGET = wiz

# Explicitly list your source files. More robust than `wildcard`.
# Add any new .cpp files you create to this list.
SRCS =  main.cpp \
        Game.cpp \
        Player.cpp \
        TextureManager.cpp \
        # Add other .cpp files here

# Automatically generate object file names from source files
OBJS = $(SRCS:.cpp=.o)
# Automatically generate dependency file names
DEPS = $(SRCS:.cpp=.d)

# --- Build Type (Debug vs. Release) ---
# Default to a Release build.
# To create a debug build, run: `make debug=1`
BUILD_TYPE = Release
ifeq ($(debug), 1)
    # Add -g flag for debugging symbols
    CXXFLAGS += -g
    BUILD_TYPE = Debug
else
    # Add -O3 flag for high optimization in release builds
    CXXFLAGS += -O3
endif

# --- Terminal Colors for Output ---
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
NC := \033[0m # No Color

# ===================================================================
# Build Rules
# ===================================================================

# The default rule: `make` or `make all` will run this.
all: $(TARGET)

# Rule to link all the object files into the final executable
$(TARGET): $(OBJS)
	@echo "$(GREEN)Linking executable: $(TARGET) ($(BUILD_TYPE) build)$(NC)"
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)Build complete! Run with ./$(TARGET)$(NC)"

# Rule to compile a .cpp source file into a .o object file
%.o: %.cpp
	@echo "$(YELLOW)Compiling: $<$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up the project directory
clean:
	@echo "$(BLUE)Cleaning up project files...$(NC)"
	rm -f $(OBJS) $(DEPS) $(TARGET)

# Include the generated dependency files.
# The '-' tells make to not complain if the file doesn't exist yet.
-include $(DEPS)

# Phony targets are not files.
.PHONY: all clean

