CXX = g++

CXXFLAGS = -std=c++20 -Wall -O2

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lGLEW -lGL

TARGET = main

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) $(LIBS)

run:
	./main

clean:
	rm -f $(TARGET)
