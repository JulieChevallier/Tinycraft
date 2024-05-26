CXX = g++
CXXFLAGS = -std=c++14 -O3 -s
LDFLAGS = -lGLEW -lGLU -lGL -lsfml-graphics -lsfml-audio -lsfml-system -lsfml-window -lSOIL -g
EXEC = TinyCraft
SRC = $(wildcard src/*.cpp) $(wildcard src/Shaders/*.cpp) $(wildcard src/Blocs/*.cpp) $(wildcard src/PerlinNoise/*.cpp) 
OBJ = $(SRC:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

run: all
	./$(EXEC)

.PHONY: all clean run
