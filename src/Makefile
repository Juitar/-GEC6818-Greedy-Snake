CXX := g++
CXXFLAGS := -Wall -g -I../include

SRC := main.cpp bmp.cpp
OBJ := $(SRC:.cpp=.o)

all: main test_bmp

main: main.o bmp.o
	$(CXX) $^ -o $@

test_bmp: test_bmp.o bmp.o
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o main test_bmp

.PHONY: all clean