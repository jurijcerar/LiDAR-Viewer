CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Iimgui
LDFLAGS = -lglfw -ldl -lGL

# All .cpp and .c files
SRC_CPP = $(wildcard src/*.cpp) $(wildcard imgui/*.cpp) $(wildcard imgui/backends/*.cpp)
SRC_C   = $(wildcard src/*.c)
OBJ_CPP = $(SRC_CPP:.cpp=.o)
OBJ_C   = $(SRC_C:.c=.o)
OBJ     = $(OBJ_CPP) $(OBJ_C)

TARGET = 3d_kruskal

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compile .cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .c files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
