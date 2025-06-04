#include "../include/Snake.h"
#include <iostream>

// 构造函数
Snake::Snake(int x, int y) : direction(Direction::RIGHT), lastDirectionChange(Direction::RIGHT), growing(false) {
    // 初始化蛇的身体，默认长度为3
    body.push_back(std::make_pair(x, y));       // 蛇头
    body.push_back(std::make_pair(x - 1, y));   // 蛇身
    body.push_back(std::make_pair(x - 2, y));   // 蛇尾
}

// 移动蛇
void Snake::move() {
    if (body.empty()) return;
    
    // 获取当前蛇头位置
    std::pair<int, int> head = body.front();
    
    // 根据方向计算新的头部位置
    std::pair<int, int> newHead;
    switch (direction) {
        case Direction::UP:
            newHead = std::make_pair(head.first, head.second - 1);
            break;
        case Direction::DOWN:
            newHead = std::make_pair(head.first, head.second + 1);
            break;
        case Direction::LEFT:
            newHead = std::make_pair(head.first - 1, head.second);
            break;
        case Direction::RIGHT:
            newHead = std::make_pair(head.first + 1, head.second);
            break;
    }
    
    // 在蛇身体前端插入新的头部
    body.insert(body.begin(), newHead);
    
    // 如果蛇不在生长状态，则移除尾部
    if (!growing) {
        body.pop_back();
    } else {
        // 重置生长状态
        growing = false;
    }
}

// 改变蛇的方向
void Snake::changeDirection(Direction newDirection) {
    // 防止蛇在同一格内回头
    // 只有当新方向与当前方向不相反时才改变方向
    
    // 检查新方向是否与当前方向相反
    bool isOppositeDirection = 
        (direction == Direction::UP && newDirection == Direction::DOWN) ||
        (direction == Direction::DOWN && newDirection == Direction::UP) ||
        (direction == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDirection == Direction::LEFT);
    
    // 如果方向相反，直接返回
    if (isOppositeDirection) {
        return;
    }
    
    // 检查是否是水平和垂直方向之间的转换
    bool isHorizontalToVertical = 
        ((direction == Direction::LEFT || direction == Direction::RIGHT) && 
         (newDirection == Direction::UP || newDirection == Direction::DOWN));
    
    bool isVerticalToHorizontal = 
        ((direction == Direction::UP || direction == Direction::DOWN) && 
         (newDirection == Direction::LEFT || newDirection == Direction::RIGHT));
    
    // 只允许垂直和水平方向之间的转换，或者在初始状态下改变方向
    if (isHorizontalToVertical || isVerticalToHorizontal) {
        // 更新方向
        direction = newDirection;
    }
}

// 使蛇生长
void Snake::grow() {
    growing = true;
}

// 检查蛇是否吃到了食物
bool Snake::checkEat(int foodX, int foodY) const {
    // 检查蛇头是否与食物位置重合
    return (body.front().first == foodX && body.front().second == foodY);
}

// 检查蛇是否撞到了自己
bool Snake::checkCollisionWithSelf() const {
    // 获取蛇头位置
    std::pair<int, int> head = getHead();
    
    // 从第二个身体部分开始检查是否与头部重叠
    for (std::size_t i = 1; i < body.size(); i++) {
        if (body[i].first == head.first && body[i].second == head.second) {
            return true;
        }
    }
    
    return false;
}

// 检查蛇是否撞到了墙
bool Snake::checkCollisionWithWall(int mapWidth, int mapHeight) const {
    // 获取蛇头位置
    std::pair<int, int> head = getHead();
    
    // 检查蛇头是否超出地图边界
    return (head.first < 0 || head.first >= mapWidth || head.second < 0 || head.second >= mapHeight);
}

// 获取蛇头位置
std::pair<int, int> Snake::getHead() const {
    return body.front();
}

// 获取蛇身体
const std::vector<std::pair<int, int>>& Snake::getBody() const {
    return body;
}

// 检查蛇是否存活
bool Snake::isAlive() const {
    return alive;
}

// 设置蛇的存活状态
void Snake::setAlive(bool status) {
    alive = status;
}

// 获取蛇的当前移动方向
Direction Snake::getDirection() const {
    return direction;
}

// 使蛇缩短（减少一个单位长度）
void Snake::shrink() {
    // 如果蛇身体长度大于1，则移除尾部
    if (body.size() > 1) {
        body.pop_back();
    }
} 