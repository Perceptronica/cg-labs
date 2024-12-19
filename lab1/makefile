CXX = g++
CXXFLAGS = -std=c++20 -Wall -g
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = executable
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

run:
	./executable

clean: rm -rf $(TARGET)
