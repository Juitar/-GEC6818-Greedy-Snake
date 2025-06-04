CC = g++
CFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lpthread

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# 源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# 目标文件
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
# 可执行文件
TARGET = $(BIN_DIR)/greedy-snake

# 默认目标
all: directories $(TARGET)

# 创建必要的目录
directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# 链接规则
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# 清理
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 运行
run: all
	$(TARGET)

.PHONY: all clean run directories
