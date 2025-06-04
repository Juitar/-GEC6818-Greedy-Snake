#include "../include/Snake.h"

// 构造函数
Snake::Snake(int startX, int startY) {
    // 初始化蛇身体，开始时蛇长度为3
    body.push_back(std::make_pair(startX, startY));       // 头部
    body.push_back(std::make_pair(startX - 1, startY));   // 身体部分
    body.push_back(std::make_pair(startX - 2, startY));   // 尾部
    
    // 初始方向为向右
    direction = Direction::RIGHT;
    
    // 初始状态为存活
    alive = true;
}

// 移动蛇
void Snake::move() {
    if (!alive) {
        return;
    }
    
    // 获取蛇头位置
    int headX = body.front().first;
    int headY = body.front().second;
    
    // 根据方向计算新的头部位置
    switch (direction) {
        case Direction::UP:
            headY--;
            break;
        case Direction::DOWN:
            headY++;
            break;
        case Direction::LEFT:
            headX--;
            break;
        case Direction::RIGHT:
            headX++;
            break;
    }
    
    // 将新的头部位置添加到身体前端
    body.insert(body.begin(), std::make_pair(headX, headY));
    
    // 移除尾部（除非蛇在生长）
    body.pop_back();
}

// 改变蛇的方向
void Snake::changeDirection(Direction newDirection) {
    // 防止蛇直接掉头（这会导致蛇撞到自己）
    if ((direction == Direction::UP && newDirection == Direction::DOWN) ||
        (direction == Direction::DOWN && newDirection == Direction::UP) ||
        (direction == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDirection == Direction::LEFT)) {
        return;
    }
    
    direction = newDirection;
}

// 增长蛇的长度
void Snake::grow() {
    // 获取尾部位置
    std::pair<int, int> tail = body.back();
    
    // 在尾部添加一个新的身体部分（位置与当前尾部相同，下一次移动时会分开）
    body.push_back(tail);
}

// 检查蛇是否吃到了食物
bool Snake::checkEat(int foodX, int foodY) const {
    // 检查蛇头是否与食物位置重合
    return (body.front().first == foodX && body.front().second == foodY);
}

// 检查蛇是否撞到了自己
bool Snake::checkCollisionWithSelf() const {
    // 获取蛇头位置
    std::pair<int, int> head = body.front();
    
    // 从第二个身体部分开始检查是否与头部重合
    for (size_t i = 1; i < body.size(); i++) {
        if (body[i].first == head.first && body[i].second == head.second) {
            return true;
        }
    }
    
    return false;
}

// 检查蛇是否撞到了墙
bool Snake::checkCollisionWithWall(int mapWidth, int mapHeight) const {
    // 获取蛇头位置
    std::pair<int, int> head = body.front();
    
    // 检查是否超出地图边界
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