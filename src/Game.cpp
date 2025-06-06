#include "../include/Game.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <fcntl.h>  // 添加fcntl.h头文件，因为inputLoop中使用了fcntl函数
#include <unistd.h> // 添加unistd.h头文件，因为STDIN_FILENO在这个头文件中定义

// 构造函数
Game::Game(int width, int height, int cellSize, const std::string& resourcePath)
    : state(GameState::PAUSED),
      map(width / cellSize, height / cellSize),
      snake(width / (2 * cellSize), height / (2 * cellSize)),
      display(width, height, cellSize),
      input(),
      screenWidth(width),
      screenHeight(height),
      cellSize(cellSize),
      gameSpeed(300),
      originalGameSpeed(300),
      pepperEffectActive(false),
      resourcePath(resourcePath),
      isGameOverDrawn(false) {  // 显式初始化
    std::srand(std::time(nullptr));
}

// 析构函数
Game::~Game() {
    // 确保所有线程已经结束
    exit();
    waitForThreads();
}

// 初始化游戏
bool Game::initialize() {
    // 初始化显示
    if (!display.initialize()) {
        std::cerr << "Failed to initialize display" << std::endl;
        return false;
    }
    
    // 加载资源 - 使用构造函数中传入的resourcePath，而不是硬编码的"resources"
    if (!display.loadResources(resourcePath)) {
        std::cerr << "Failed to load resources" << std::endl;
        return false;
    }
    
    // 初始化输入
    if (!input.initialize()) {
        std::cerr << "Failed to initialize input" << std::endl;
        return false;
    } else {
        std::cout << "Input device: initialized" << std::endl;
        
        // 启动触屏输入线程
        std::thread* inputThread = input.startInputThread();
        if (inputThread) {
            inputThread->detach();  // 分离线程，使其独立运行
        }
    }
    
    // 生成初始食物
    generateFood();
    
    // 更新地图
    updateMap();
    
    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

// 开始游戏
void Game::start() {
    // 如果游戏已经在运行，直接返回
    if (state == GameState::RUNNING) {
        return;
    }
    
    // 设置游戏状态为运行
    state = GameState::RUNNING;
    
    // 启动游戏线程
    gameThread = std::thread(&Game::gameLoop, this);
    renderThread = std::thread(&Game::renderLoop, this);
    inputThread = std::thread(&Game::inputLoop, this);
}

// 暂停游戏
void Game::pause() {
    if (state == GameState::RUNNING) {
        state = GameState::PAUSED;
    }
}

// 继续游戏
void Game::resume() {
    if (state == GameState::PAUSED) {
        state = GameState::RUNNING;
    }
}

// 结束游戏
void Game::exit() {
    state = GameState::EXIT;
}

// 重置游戏
void Game::reset() {
    // 确保游戏已经停止
    if (state == GameState::RUNNING) {
        pause();
    }
    
    // 重置蛇
    snake = Snake(map.getWidth() / 2, map.getHeight() / 2);
    
    // 清空食物列表
    foods.clear();
    
    // 重新生成食物
    generateFood();
    
    // 更新地图
    updateMap();
}

// 等待所有线程结束
void Game::waitForThreads() {
    if (gameThread.joinable()) {
        gameThread.join();
    }
    
    if (renderThread.joinable()) {
        renderThread.join();
    }
    
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

// 获取当前游戏状态
GameState Game::getState() const {
    return state;
}

// 游戏主循环
void Game::gameLoop() {
    // 记录上一次蛇头位置，用于调试
    std::pair<int, int> lastHeadPos = snake.getHead();
    int moveCount = 0;
    
    while (state != GameState::EXIT) {
        // 如果游戏暂停，等待
        if (state == GameState::PAUSED) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        if (state == GameState::GAME_OVER) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // 更新游戏状态
        update();
        
        // 检查蛇是否撞到墙
        if (snake.checkCollisionWithWall(map.getWidth(), map.getHeight())) {
            state = GameState::GAME_OVER;
            display.drawGameOver();  // 直接调用绘制
            display.update();
        }
        // 处理碰撞（包括蛇与自身的碰撞和食物碰撞）
        handleCollisions();
        
        // 立即检查蛇是否碰撞，如果碰撞立即停止
        if (state == GameState::GAME_OVER) {
            // 直接绘制游戏结束画面
            display.drawGameOver();
            display.update();
            
            // 等待3秒让玩家看到游戏结束画面
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        
        // 更新地图
        updateMap();
        
        // 控制游戏速度
        std::this_thread::sleep_for(std::chrono::milliseconds(gameSpeed));
        }
}


// 渲染循环
void Game::renderLoop() {
    // 帧率控制
    const int targetFPS = 15; // 降低帧率，减少闪烁
    const std::chrono::milliseconds frameTime(1000 / targetFPS);
    std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
    
    while (state != GameState::EXIT) {
        // 计算帧时间
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
        
        // 如果距离上一帧时间不足，等待
        if (elapsedTime < frameTime) {
            std::this_thread::sleep_for(frameTime - elapsedTime);
            continue;
        }
        
        // 更新上一帧时间
        lastFrameTime = std::chrono::steady_clock::now();
        
        // 获取锁，确保在渲染时不会修改游戏状态
        std::lock_guard<std::mutex> lock(gameMutex);
        
        // 避免每帧都清空屏幕，利用Display类的背景缓存机制
        
        // 绘制地图（这会利用背景缓存）
        display.drawMap(&map);
        
        // 绘制所有食物
        for (const auto& foodWithLifetime : foods) {
            display.drawFood(&(foodWithLifetime.food));
        }
        
        // 绘制蛇（最后绘制蛇，确保蛇覆盖在其他元素上方）
        display.drawSnake(&snake);
        
        // 如果游戏结束，绘制游戏结束状态
        if (state == GameState::GAME_OVER && !isGameOverDrawn) {
            display.drawGameOver();
            isGameOverDrawn = true;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}

// 输入处理循环
void Game::inputLoop() {
    Direction lastProcessedDirection = snake.getDirection();
    bool directionChangedThisCycle = false;
    InputEvent event;
    
    while (state != GameState::EXIT) {
        // 处理键盘输入
        input.processKeyboardInput();
        
        // 处理触屏输入
        if (input.getEvent(event)) {
            // 如果是移动事件，转换为方向
            if (event.type == InputEventType::TOUCH_MOVE) {
                Direction touchDirection = input.convertEventToDirection(event, snake.getDirection());
                input.setDirection(touchDirection);
            }
        }
        
        // 获取新的方向
        Direction newDirection = input.getDirection();
        
        // 检查是否有新的输入
        if (input.hasNewInput()) {
            // 获取锁，确保在处理输入时不会更新游戏状态
            std::lock_guard<std::mutex> lock(gameMutex);
            
            // 只有当方向发生变化，且本周期内未改变过方向时才改变蛇的方向
            if (newDirection != lastProcessedDirection && !directionChangedThisCycle) {
                // 改变蛇的方向
                snake.changeDirection(newDirection);
                
                // 更新上一次处理的方向
                lastProcessedDirection = newDirection;
                
                // 标记本周期已改变方向
                directionChangedThisCycle = true;
            }
            
            // 清除新输入标记
            input.clearNewInput();
        }
        
        // 在每个游戏帧开始时重置方向改变标记
        if (state == GameState::RUNNING) {
            // 获取锁
            std::lock_guard<std::mutex> lock(gameMutex);
            directionChangedThisCycle = false;
        }
        
        // 控制输入处理速度
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// 更新游戏状态
void Game::update() {
    // 获取锁，确保在更新时不会渲染
    std::lock_guard<std::mutex> lock(gameMutex);
    
    // 移动蛇
    snake.move();
    
    // 检查蛇的长度，如果为0则游戏结束
    if (snake.getBody().empty()) {
        std::cout << "Snake has no length! Game over." << std::endl;
        state = GameState::GAME_OVER;
        return;
    }
    
    // 检查辣椒效果是否结束
    checkPepperEffect();
    
    // 检查和移除过期食物
    checkAndRemoveExpiredFoods();
    
    // 根据蛇的长度调整游戏速度，长度越长速度越快，但有最低速度限制
    // 只有在没有辣椒效果时才调整速度
    if (!pepperEffectActive) {
        int snakeLength = static_cast<int>(snake.getBody().size());
        int newSpeed = std::max(150, 400 - (snakeLength - 3) * 10);
        if (newSpeed != gameSpeed) {
            gameSpeed = newSpeed;
            originalGameSpeed = newSpeed; // 同时更新原始速度
        }
    }
}

// 处理碰撞
void Game::handleCollisions() {
    // 获取锁，确保在处理碰撞时不会渲染
    std::lock_guard<std::mutex> lock(gameMutex);
    
    // 检查蛇是否撞到自己
    if (snake.checkCollisionWithSelf()) {
        std::cout << "Snake hit itself! Game over." << std::endl;
        state = GameState::GAME_OVER;
        return; // 立即返回，不再处理其他碰撞
    }
    
    // 检查蛇是否吃到食物
    bool foodEaten = false;
    auto it = foods.begin();
    
    while (it != foods.end()) {
        if (snake.checkEat(it->food.getX(), it->food.getY())) {
            foodEaten = true;
            
            // 根据食物类型处理不同的效果
            FoodType foodType = it->food.getType();
            
            switch (foodType) {
                case FoodType::APPLE:
                    // 苹果：蛇增长一个单位
                    snake.grow();
                    break;
                    
                case FoodType::PEPPER:
                    // 辣椒：蛇增长一个单位，短时间内增加移动速度
                    snake.grow();
                    // 如果辣椒效果未激活，保存原始速度
                    if (!pepperEffectActive) {
                        originalGameSpeed = gameSpeed;
                    }
                    // 将游戏速度减半（移动更快）
                    gameSpeed = originalGameSpeed / 2;
                    // 设置辣椒效果持续10秒
                    pepperEffectEndTime = std::chrono::steady_clock::now() + std::chrono::seconds(10);
                    pepperEffectActive = true;
                    break;
                    
                case FoodType::MEAT:
                    // 肉：蛇增长两个单位
                    snake.grow();
                    snake.grow();
                    break;
                    
                case FoodType::BOMB:
                    // 炸弹：蛇减少两个单位
                    // 减少两次长度
                    for (int i = 0; i < 2; i++) {
                        // 检查蛇的长度，如果只有一个单位了，游戏结束
                        if (snake.getBody().size() <= 1) {
                            std::cout << "Snake ate a bomb and disappeared! Game over." << std::endl;
                            state = GameState::GAME_OVER;
                            return;
                        }
                        // 减少蛇的长度
                        snake.shrink();
                    }
                    break;
            }
            
            // 从食物列表中移除被吃掉的食物
            it = foods.erase(it);
            
            // 直接在这里生成新食物，而不是调用generateFood方法
            // 这样可以避免死锁，因为handleCollisions方法已经获取了gameMutex锁
            int attempts = 0;
            const int maxAttempts = 50;
            
            while (foods.size() < maxFoods && attempts < maxAttempts) {
                attempts++;
                
                // 创建新食物
                Food newFood;
                newFood.generate(map, snake);
                
                // 检查新食物的位置是否与现有食物重叠
                bool overlapping = false;
                for (const auto& existingFood : foods) {
                    if (newFood.getX() == existingFood.food.getX() && 
                        newFood.getY() == existingFood.food.getY()) {
                        overlapping = true;
                        break;
                    }
                }
                
                // 如果新食物与现有食物重叠，重新生成
                if (overlapping) {
                    continue;
                }
                
                // 设置食物的过期时间
                auto expirationTime = std::chrono::steady_clock::now() + std::chrono::seconds(foodLifetime);
                
                // 添加到食物列表
                foods.push_back({newFood, expirationTime});
            }
            
            // 一次只处理一个食物碰撞
            break;
        } else {
            ++it;
        }
    }
}



// 检查辣椒效果是否结束
void Game::checkPepperEffect() {
    // 如果辣椒效果激活，检查是否已经结束
    if (pepperEffectActive) {
        auto now = std::chrono::steady_clock::now();
        if (now >= pepperEffectEndTime) {
            // 辣椒效果结束，恢复原始速度
            gameSpeed = originalGameSpeed;
            pepperEffectActive = false;
        }
    }
}

// 更新地图
void Game::updateMap() {
    // 获取锁，确保在更新地图时不会渲染
    std::lock_guard<std::mutex> lock(gameMutex);
    
    // 清空地图
    map.clear();
    
    // 更新所有食物位置
    for (const auto& foodWithLifetime : foods) {
        foodWithLifetime.food.updateMap(map);
    }
    
    // 更新蛇在地图上的位置
    const auto& body = snake.getBody();
    
    // 设置蛇头
    if (!body.empty()) {
        map.setElement(body[0].first, body[0].second, MapElementType::SNAKE_HEAD);
        
        // 设置蛇身
        for (std::size_t i = 1; i < body.size(); i++) {
            map.setElement(body[i].first, body[i].second, MapElementType::SNAKE_BODY);
        }
    }
}

// 生成食物
void Game::generateFood() {
    // 获取锁，确保在生成食物时不会渲染
    std::lock_guard<std::mutex> lock(gameMutex);
    
    // 检查当前食物数量，如果少于最大值，则生成新食物
    int attempts = 0;
    const int maxAttempts = 50; // 最大尝试次数，避免无限循环
    
    while (foods.size() < maxFoods && attempts < maxAttempts) {
        attempts++;
        
        // 创建新食物
        Food newFood;
        newFood.generate(map, snake);
        
        // 检查新食物的位置是否与现有食物重叠
        bool overlapping = false;
        for (const auto& existingFood : foods) {
            if (newFood.getX() == existingFood.food.getX() && 
                newFood.getY() == existingFood.food.getY()) {
                overlapping = true;
                break;
            }
        }
        
        // 如果新食物与现有食物重叠，重新生成
        if (overlapping) {
            continue;
        }
        
        // 设置食物的过期时间
        auto expirationTime = std::chrono::steady_clock::now() + std::chrono::seconds(foodLifetime);
        
        // 添加到食物列表
        foods.push_back({newFood, expirationTime});
    }
}

// 检查和移除过期食物
void Game::checkAndRemoveExpiredFoods() {
    // 注意：不需要获取锁，因为调用此方法的update()已经获取了锁
    
    // 检查每个食物是否过期
    auto it = foods.begin();
    while (it != foods.end()) {
        if (it->isExpired()) {
            it = foods.erase(it);
        } else {
            ++it;
        }
    }
    
    // 如果食物数量少于最大值，生成新食物
    if (foods.size() < maxFoods) {
        // 直接在这里生成食物，而不是调用generateFood方法
        // 因为调用此方法的update()已经获取了锁
        int attempts = 0;
        const int maxAttempts = 50;
        
        while (foods.size() < maxFoods && attempts < maxAttempts) {
            attempts++;
            
            // 创建新食物
            Food newFood;
            newFood.generate(map, snake);
            
            // 检查新食物的位置是否与现有食物重叠
            bool overlapping = false;
            for (const auto& existingFood : foods) {
                if (newFood.getX() == existingFood.food.getX() && 
                    newFood.getY() == existingFood.food.getY()) {
                    overlapping = true;
                    break;
                }
            }
            
            // 如果新食物与现有食物重叠，重新生成
            if (overlapping) {
                continue;
            }
            
            // 设置食物的过期时间
            auto expirationTime = std::chrono::steady_clock::now() + std::chrono::seconds(foodLifetime);
            
            // 添加到食物列表
            foods.push_back({newFood, expirationTime});
        }
    }
}