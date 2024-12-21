CXX = g++

CXXFLAGS = -Wall -std=c++11

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lGLEW -lGL

INCLUDES = -I/usr/include

SRC = src/main.cpp

OBJ = $(SRC:.cpp=.o)

EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)

run:
	./main
