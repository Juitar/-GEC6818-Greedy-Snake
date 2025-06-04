#ifndef FOOD_H
#define FOOD_H

#include "Map.h"
#include "Snake.h"
#include <cstdlib>
#include <ctime>

// 食物类型枚举
enum class FoodType {
    APPLE,   // 苹果：加一个长度
    PEPPER,  // 辣椒：加一个长度，短时间内增加移动速度
    MEAT,    // 肉：加两个长度
    BOMB     // 炸弹：减两个长度
};

class Food {
private:
    int x;  // 食物的X坐标
    int y;  // 食物的Y坐标
    FoodType type;  // 食物类型

public:
    // 构造函数
    Food(int x = 0, int y = 0);
    
    // 获取食物的X坐标
    int getX() const;
    
    // 获取食物的Y坐标
    int getY() const;
    
    // 获取食物类型
    FoodType getType() const;
    
    // 在地图上随机生成食物，避开蛇的位置
    void generate(const Map& map, const Snake& snake);
    
    // 更新地图上的食物位置
    void updateMap(Map& map) const;
};

#endif // FOOD_H