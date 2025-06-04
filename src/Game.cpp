#include "../include/Game.h"
#include <thread>
#include <chrono>

// 构造函数
Game::Game(int mapWidth, int mapHeight) {
    // 创建地图对象
    map = new Map(mapWidth, mapHeight);
    
    // 创建蛇对象，初始位置在地图中央
    snake = new Snake(mapWidth / 2, mapHeight / 2);
    
    // 创建食物对象
    food = new Food();
    
    // 初始化游戏状态
    state = GameState::PAUSED;
    
    // 初始化游戏分数
    score = 0;
    
    // 初始化游戏速度和难度
    speed = 200; // 毫秒
    difficulty = 1;
}

// 析构函数
Game::~Game() {
    delete snake;
    delete food;
    delete map;
}

// 初始化游戏
void Game::initialize() {
    // 初始化地图
    map->initialize();
    
    // 生成食物
    food->generate(map->getWidth(), map->getHeight(), snake->getBody());
    
    // 重置游戏分数
    score = 0;
    
    // 设置游戏状态为暂停
    state = GameState::PAUSED;
}

// 开始游戏
void Game::start() {
    // 设置游戏状态为运行
    state = GameState::RUNNING;
}

// 暂停游戏
void Game::pause() {
    // 设置游戏状态为暂停
    state = GameState::PAUSED;
}

// 恢复游戏
void Game::resume() {
    // 设置游戏状态为运行
    state = GameState::RUNNING;
}

// 结束游戏
void Game::end() {
    // 设置游戏状态为结束
    state = GameState::GAME_OVER;
}

// 处理输入
void Game::handleInput(Direction direction) {
    // 改变蛇的方向
    snake->changeDirection(direction);
}

// 更新游戏状态
void Game::update() {
    // 如果游戏不在运行状态，直接返回
    if (state != GameState::RUNNING) {
        return;
    }
    
    // 移动蛇
    snake->move();
    
    // 检查蛇是否吃到食物
    if (snake->checkEat(food->getX(), food->getY())) {
        // 蛇增长
        snake->grow();
        
        // 增加分数（根据食物类型）
        score += (food->getType() + 1) * 10;
        
        // 生成新的食物
        food->generate(map->getWidth(), map->getHeight(), snake->getBody());
        
        // 根据分数调整游戏速度
        if (score % 100 == 0 && speed > 50) {
            speed -= 10;
        }
    }
    
    // 检查蛇是否撞到墙或自己
    if (snake->checkCollisionWithWall(map->getWidth(), map->getHeight()) || 
        snake->checkCollisionWithSelf()) {
        // 设置蛇为死亡状态
        snake->setAlive(false);
        
        // 结束游戏
        end();
    }
    
    // 更新地图
    updateMap();
}

// 更新地图上的元素
void Game::updateMap() {
    // 清空地图
    map->clear();
    
    // 初始化地图边界
    map->initialize();
    
    // 设置食物位置
    map->setElement(food->getX(), food->getY(), MapElementType::FOOD);
    
    // 设置蛇的位置
    const auto& snakeBody = snake->getBody();
    
    // 设置蛇头
    map->setElement(snakeBody[0].first, snakeBody[0].second, MapElementType::SNAKE_HEAD);
    
    // 设置蛇身
    for (size_t i = 1; i < snakeBody.size(); i++) {
        map->setElement(snakeBody[i].first, snakeBody[i].second, MapElementType::SNAKE_BODY);
    }
}

// 渲染游戏画面（预留接口，后续接入图像显示）
void Game::render() {
    // 这里将来会实现图像显示功能
    // 目前为空实现
}

// 获取游戏状态
GameState Game::getState() const {
    return state;
}

// 获取游戏分数
int Game::getScore() const {
    return score;
}

// 设置游戏难度
void Game::setDifficulty(int diff) {
    difficulty = diff;
    
    // 根据难度调整速度
    switch (difficulty) {
        case 1: // 简单
            speed = 200;
            break;
        case 2: // 中等
            speed = 150;
            break;
        case 3: // 困难
            speed = 100;
            break;
    }
}

// 重置游戏
void Game::reset() {
    // 删除旧的对象
    delete snake;
    
    // 创建新的蛇对象
    snake = new Snake(map->getWidth() / 2, map->getHeight() / 2);
    
    // 初始化游戏
    initialize();
} 