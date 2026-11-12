CXX := g++
CXXFLAGS := -Wall -Wextra -Iinclude -MMD -MP

SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET := build/app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@

build/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf build