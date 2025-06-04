#ifndef FOOD_H
#define FOOD_H

#include <utility>
#include <vector>

// 食物类
class Food {
private:
    // 食物位置
    int x;
    int y;
    // 食物类型（可以扩展为不同类型的食物有不同效果）
    int type;

public:
    // 构造函数
    Food();
    
    // 在地图上随机生成食物，避开蛇的身体
    void generate(int mapWidth, int mapHeight, const std::vector<std::pair<int, int>>& snakeBody);
    
    // 获取食物位置
    int getX() const;
    int getY() const;
    
    // 获取食物类型
    int getType() const;
};

#endif // FOOD_H 