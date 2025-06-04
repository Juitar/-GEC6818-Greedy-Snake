#include "../include/Food.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// 构造函数
Food::Food() {
    // 初始化随机数生成器
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // 初始化食物位置为无效位置
    x = -1;
    y = -1;
    
    // 默认食物类型为0
    type = 0;
}

// 在地图上随机生成食物，避开蛇的身体
void Food::generate(int mapWidth, int mapHeight, const std::vector<std::pair<int, int>>& snakeBody) {
    // 创建一个可能的位置列表
    std::vector<std::pair<int, int>> possiblePositions;
    
    // 遍历地图上的所有位置
    for (int i = 0; i < mapWidth; i++) {
        for (int j = 0; j < mapHeight; j++) {
            // 检查该位置是否被蛇占用
            bool isOccupied = false;
            for (const auto& bodyPart : snakeBody) {
                if (bodyPart.first == i && bodyPart.second == j) {
                    isOccupied = true;
                    break;
                }
            }
            
            // 如果没有被占用，添加到可能的位置列表
            if (!isOccupied) {
                possiblePositions.push_back(std::make_pair(i, j));
            }
        }
    }
    
    // 如果有可能的位置，随机选择一个
    if (!possiblePositions.empty()) {
        int randomIndex = std::rand() % possiblePositions.size();
        x = possiblePositions[randomIndex].first;
        y = possiblePositions[randomIndex].second;
        
        // 随机生成食物类型（0-2）
        type = std::rand() % 3;
    }
}

// 获取食物位置
int Food::getX() const {
    return x;
}

int Food::getY() const {
    return y;
}

// 获取食物类型
int Food::getType() const {
    return type;
} 