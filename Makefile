CXX      = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Iinclude -Iimgui

# Detect OS
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lglfw3 -lopengl32 -lgdi32 -lcomdlg32
    TARGET  = lidar_viewer.exe
else
    LDFLAGS = -lglfw -ldl -lGL -lpthread
    TARGET  = lidar_viewer
endif

SRC_CPP = $(wildcard src/*.cpp) $(wildcard imgui/*.cpp)
SRC_C   = $(wildcard src/*.c)
OBJ     = $(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
