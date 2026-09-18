CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
INCLUDES = -I./imgui -I./imgui/backends
LIBS = -lSDL2 -lGL -lm -ldl -lpthread

IMGUI_SRC = imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui_demo.cpp
BACKEND_SRC = imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp
APP_SRC = main.cpp

TARGET = menu

all: $(TARGET)

$(TARGET): $(IMGUI_SRC:.cpp=.o) $(BACKEND_SRC:.cpp=.o) $(APP_SRC:.cpp=.o)
	$(CXX) -o $@ $^ $(LIBS)

imgui/%.o: imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

imgui/backends/%.o: imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I/data/data/com.termux/files/usr/include/SDL2 -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I/data/data/com.termux/files/usr/include/SDL2 -c $< -o $@

clean:
	rm -f $(TARGET) *.o imgui/*.o imgui/backends/*.o

run: $(TARGET)
	./$(TARGET)
