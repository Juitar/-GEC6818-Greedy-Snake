#include "../include/Game.h"
#include <thread>
#include <chrono>
#include <iostream>

// 初始化静态变量
std::atomic<bool> Game::isGameOver(false);

// 构造函数
Game::Game(int mapWidth, int mapHeight) {
    // 创建地图对象
    map = new Map(mapWidth, mapHeight);
    
    // 创建蛇对象，初始位置在地图中央
    snake = new Snake(mapWidth / 2, mapHeight / 2);
    
    // 创建食物对象
    food = new Food();
    
    // 创建显示对象（预留接口）
    display = nullptr;  // 后续实现
    
    // 创建输入对象
    input = new Input("/dev/input/event0");  // 触摸屏设备路径
    
    // 初始化游戏状态
    state = GameState::PAUSED;
    
    // 初始化游戏分数
    score = 0;
    
    // 初始化游戏速度和难度
    speed = 200; // 毫秒
    difficulty = 1;
    
    // 初始化线程相关变量
    gameRunning = false;
    
    // 初始化线程指针
    inputThread = nullptr;
    gameLoopThread = nullptr;
    renderThread = nullptr;
    foodThread = nullptr;
}

// 析构函数
Game::~Game() {
    // 确保游戏已经结束
    end();
    
    // 等待所有线程结束
    waitForThreads();
    
    // 释放资源
    delete snake;
    delete food;
    delete map;
    
    if (display != nullptr) {
        delete display;
    }
    
    if (input != nullptr) {
        delete input;
    }
    
    // 释放线程资源
    if (inputThread != nullptr) {
        delete inputThread;
    }
    
    if (gameLoopThread != nullptr) {
        delete gameLoopThread;
    }
    
    if (renderThread != nullptr) {
        delete renderThread;
    }
    
    if (foodThread != nullptr) {
        delete foodThread;
    }
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
    
    // 重置游戏运行标志
    gameRunning = false;
    isGameOver = false;
    
    // 更新地图
    updateMap();
    
    // 初始化输入设备
    if (input != nullptr) {
        input->initialize();
    }
}

// 开始游戏
void Game::start() {
    // 设置游戏状态为运行
    state = GameState::RUNNING;
    gameRunning = true;
    
    // 启动线程
    if (gameLoopThread == nullptr) {
        gameLoopThread = new std::thread(&Game::gameLoop, this);
    }
    
    if (renderThread == nullptr) {
        renderThread = new std::thread(&Game::renderLoop, this);
    }
    
    if (foodThread == nullptr) {
        foodThread = new std::thread(&Game::foodManagementLoop, this);
    }
    
    if (input != nullptr && inputThread == nullptr) {
        inputThread = new std::thread(&Game::inputLoop, this);
    }
    
    // 通知所有等待的线程
    gameCondVar.notify_all();
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
    
    // 通知所有等待的线程
    gameCondVar.notify_all();
}

// 结束游戏
void Game::end() {
    // 设置游戏状态为结束
    state = GameState::EXIT;
    gameRunning = false;
    isGameOver = true;
    
    // 通知所有等待的线程
    gameCondVar.notify_all();
}

// 等待所有线程结束
void Game::waitForThreads() {
    if (inputThread != nullptr && inputThread->joinable()) {
        inputThread->join();
    }
    
    if (gameLoopThread != nullptr && gameLoopThread->joinable()) {
        gameLoopThread->join();
    }
    
    if (renderThread != nullptr && renderThread->joinable()) {
        renderThread->join();
    }
    
    if (foodThread != nullptr && foodThread->joinable()) {
        foodThread->join();
    }
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
        isGameOver = true;
        state = GameState::GAME_OVER;
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
    // 如果显示对象存在，则调用其绘制方法
    if (display != nullptr) {
        display->drawMap(map);
        display->drawSnake(snake);
        display->drawFood(food);
        display->drawScore(score);
        display->drawGameState(state);
        display->update();
    } else {
        // 控制台输出（临时方案）
        std::cout << "Score: " << score << std::endl;
    }
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

// 输入处理循环
void Game::inputLoop() {
    while (gameRunning) {
        // 如果游戏暂停，等待恢复
        if (state == GameState::PAUSED) {
            std::unique_lock<std::mutex> lock(gameMutex);
            gameCondVar.wait(lock, [this] { return state != GameState::PAUSED || !gameRunning; });
        }
        
        // 如果游戏结束，退出循环
        if (state == GameState::EXIT || state == GameState::GAME_OVER || !gameRunning) {
            break;
        }
        
        // 获取输入事件
        if (input != nullptr) {
            InputEvent event;
            if (input->getEvent(event)) {
                // 将输入事件转换为方向
                Direction dir = input->convertEventToDirection(event, snake->getDirection());
                handleInput(dir);
            }
        }
        
        // 控制循环频率
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

// 游戏主循环
void Game::gameLoop() {
    while (gameRunning) {
        // 如果游戏暂停，等待恢复
        if (state == GameState::PAUSED) {
            std::unique_lock<std::mutex> lock(gameMutex);
            gameCondVar.wait(lock, [this] { return state != GameState::PAUSED || !gameRunning; });
        }
        
        // 如果游戏结束，退出循环
        if (state == GameState::EXIT || state == GameState::GAME_OVER || !gameRunning) {
            break;
        }
        
        // 更新游戏状态
        update();
        
        // 控制游戏速度
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}

// 渲染循环
void Game::renderLoop() {
    while (gameRunning) {
        // 如果游戏暂停，等待恢复
        if (state == GameState::PAUSED) {
            std::unique_lock<std::mutex> lock(gameMutex);
            gameCondVar.wait(lock, [this] { return state != GameState::PAUSED || !gameRunning; });
        }
        
        // 如果游戏结束，退出循环
        if (state == GameState::EXIT || !gameRunning) {
            break;
        }
        
        // 渲染游戏画面
        render();
        
        // 控制渲染频率
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// 食物管理循环
void Game::foodManagementLoop() {
    while (gameRunning) {
        // 如果游戏暂停，等待恢复
        if (state == GameState::PAUSED) {
            std::unique_lock<std::mutex> lock(gameMutex);
            gameCondVar.wait(lock, [this] { return state != GameState::PAUSED || !gameRunning; });
        }
        
        // 如果游戏结束，退出循环
        if (state == GameState::EXIT || state == GameState::GAME_OVER || !gameRunning) {
            break;
        }
        
        // 如果需要生成新的食物（例如，当前没有食物）
        if (food->getX() < 0 || food->getY() < 0) {
            food->generate(map->getWidth(), map->getHeight(), snake->getBody());
        }
        
        // 控制循环频率
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
} 