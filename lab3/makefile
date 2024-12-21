CXX = g++

CXXFLAGS = -std=c++11 -Wall -O2

LIBS = -lGL -lGLEW -lsfml-window -lsfml-system -lsfml-graphics -lGLU

TARGETS = main add

SRCS_main = main.cpp
SRCS_add = add.cpp

OBJS_main = $(SRCS_main:.cpp=.o)
OBJS_add = $(SRCS_add:.cpp=.o)

all: $(TARGET)

main: $(OBJS_main)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

add: $(OBJS_add)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_main) $(OBJS_add) $(TARGETS)

.PHONY: all clean run run_add

run: main
	./main

run_add: add
	./add
