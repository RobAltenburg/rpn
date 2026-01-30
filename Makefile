CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = rpn
SRCS = main.cpp rpn.cpp operators.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp rpn.h operators.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	./test_rpn.sh

.PHONY: all clean test
