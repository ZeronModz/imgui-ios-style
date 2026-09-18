# ImGui iOS Style Project - Makefile
# For Android/Termux builds

# Compiler
CXX = g++
CC = gcc

# Flags
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic
CFLAGS = -O2 -Wall -Wextra -Wpedantic

# Include paths
INCLUDES = -I./imgui -I./backends -I/data/data/com.termux/files/usr/include/SDL2 -I/data/data/com.termux/files/usr/include

# Libraries
LIBS = -lSDL2 -lGL -lm -ldl -lpthread

# Source files
IMGUI_SRC = ./imgui/imgui.cpp \
            ./imgui/imgui_draw.cpp \
            ./imgui/imgui_tables.cpp \
            ./imgui/imgui_widgets.cpp \
            ./imgui/imgui_demo.cpp

BACKEND_SRC = ./imgui/backends/imgui_impl_sdl2.cpp \
              ./imgui/backends/imgui_impl_opengl3.cpp

APP_SRC = ./main.cpp \
          ./ios_theme.cpp

# Object files
IMGUI_OBJ = $(IMGUI_SRC:.cpp=.o)
BACKEND_OBJ = $(BACKEND_SRC:.cpp=.o)
APP_OBJ = $(APP_SRC:.cpp=.o)

# Backend build rule
./imgui/backends/%.o: ./imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Target
TARGET = ios_style_demo

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(IMGUI_OBJ) $(BACKEND_OBJ) $(APP_OBJ)
	$(CXX) -o $@ $^ $(LIBS)
	@echo "Build complete: $(TARGET)"

# Compile ImGui
./imgui/%.o: ./imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile backends
./backends/%.o: ./backends/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile app
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(IMGUI_OBJ) $(BACKEND_OBJ) $(APP_OBJ) $(TARGET)
	@echo "Clean complete"

# Run
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean run debug release install
