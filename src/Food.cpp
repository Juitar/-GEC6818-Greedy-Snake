#include "../include/Food.h"
#include <iostream>
#include <random>
#include <chrono>

// 创建一个全局的随机数生成器
namespace {
    std::mt19937 rng;
    bool initialized = false;
    
    void initializeRNG() {
        if (!initialized) {
            // 使用当前时间作为种子
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            rng.seed(seed);
            initialized = true;
        }
    }
}

// 构造函数
Food::Food(int x, int y) : x(x), y(y), type(FoodType::APPLE) {
    // 确保随机数生成器已初始化
    initializeRNG();
}

// 获取食物的X坐标
int Food::getX() const {
    return x;
}

// 获取食物的Y坐标
int Food::getY() const {
    return y;
}

// 获取食物类型
FoodType Food::getType() const {
    return type;
}

// 在地图上随机生成食物
void Food::generate(const Map& map, const Snake& snake) {
    int mapWidth = map.getWidth();
    int mapHeight = map.getHeight();
    
    // 获取蛇身体
    const auto& snakeBody = snake.getBody();
    
    // 创建一个可用位置的列表
    std::vector<std::pair<int, int>> availablePositions;
    
    // 遍历地图，找出所有可用位置
    for (int y = 1; y < mapHeight - 1; y++) {
        for (int x = 1; x < mapWidth - 1; x++) {
            // 检查位置是否为空
            if (map.getElement(x, y) == MapElementType::EMPTY) {
                // 检查位置是否不在蛇身上
                bool onSnake = false;
                for (const auto& segment : snakeBody) {
                    if (segment.first == x && segment.second == y) {
                        onSnake = true;
                        break;
                    }
                }
                
                if (!onSnake) {
                    availablePositions.push_back(std::make_pair(x, y));
                }
            }
        }
    }
    
    // 如果没有可用位置，返回
    if (availablePositions.empty()) {
        return;
    }
    
    // 随机选择一个可用位置
    std::uniform_int_distribution<int> positionDist(0, availablePositions.size() - 1);
    int randomIndex = positionDist(rng);
    x = availablePositions[randomIndex].first;
    y = availablePositions[randomIndex].second;
    
    // 随机生成食物类型
    std::uniform_int_distribution<int> typeDist(0, 99);
    int foodTypeRand = typeDist(rng);
    if (foodTypeRand < 60) {
        // 60%概率生成苹果
        type = FoodType::APPLE;
    } else if (foodTypeRand < 80) {
        // 20%概率生成辣椒
        type = FoodType::PEPPER;
    } else if (foodTypeRand < 95) {
        // 15%概率生成肉
        type = FoodType::MEAT;
    } else {
        // 5%概率生成炸弹
        type = FoodType::BOMB;
    }
}

// 在地图上更新食物的位置
void Food::updateMap(Map& map) const {
    // 根据食物类型设置不同的地图元素类型
    MapElementType elementType;
    
    switch (type) {
        case FoodType::APPLE:
            elementType = MapElementType::FOOD_APPLE;
            break;
        case FoodType::PEPPER:
            elementType = MapElementType::FOOD_PEPPER;
            break;
        case FoodType::MEAT:
            elementType = MapElementType::FOOD_MEAT;
            break;
        case FoodType::BOMB:
            elementType = MapElementType::FOOD_BOMB;
            break;
        default:
            elementType = MapElementType::FOOD_APPLE;
            break;
    }
    
    map.setElement(x, y, elementType);
}