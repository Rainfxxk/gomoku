CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -g

SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)
OBJS = $(SRCS:.cpp=.o)
TARGET = build/gomoku

all: $(TARGET)
	@./$(TARGET)

build:
	@mkdir -p $@

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS) | build
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译 .cpp 文件生成 .o 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
