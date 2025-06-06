#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <utility>

// 方向枚举
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 蛇类
class Snake {
private:
    // 蛇身体，每个部分用坐标表示
    std::vector<std::pair<int, int>> body;
    // 蛇移动方向
    Direction direction;
    // 上一次尝试改变的方向
    Direction lastDirectionChange;
    // 蛇是否存活
    bool alive;
    // 标记蛇是否正在生长
    bool growing;

public:
    // 构造函数
    Snake(int startX, int startY);
    
    // 移动蛇
    void move();
    
    // 改变蛇的方向
    void changeDirection(Direction newDirection);
    
    // 增长蛇的长度
    void grow();
    
    // 使蛇缩短（减少一个单位长度）
    void shrink();
    
    // 检查蛇是否吃到了食物
    bool checkEat(int foodX, int foodY) const;
    
    // 检查蛇是否撞到了自己
    bool checkCollisionWithSelf() const;
    
    // 检查蛇是否撞到了墙
    bool checkCollisionWithWall(int mapWidth, int mapHeight) const;
    
    // 获取蛇头位置
    std::pair<int, int> getHead() const;
    
    // 获取蛇身体
    const std::vector<std::pair<int, int>>& getBody() const;
    
    // 检查蛇是否存活
    bool isAlive() const;
    
    // 设置蛇的存活状态
    void setAlive(bool status);
    
    // 获取蛇的当前移动方向
    Direction getDirection() const;
};

#endif // SNAKE_H 