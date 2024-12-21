CXX = g++

CXXFLAGS = -std=c++17 -Wall -O2

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lGLEW -lGL -lGLU

INCLUDES = -I/usr/include -I/usr/local/include

LDFLAGS = -L/usr/lib -L/usr/local/lib

all: raytracer

raytracer: main.o
	$(CXX) $(CXXFLAGS) -o raytracer main.o $(LDFLAGS) $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp

clean:
	rm -f *.o raytracer

run:
	./raytracer
