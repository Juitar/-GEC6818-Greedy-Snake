#ifndef GAME_H
#define GAME_H

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include "Map.h"
#include "Snake.h"
#include "Food.h"
#include "Display.h"
#include "Input.h"

// 游戏状态枚举
enum class GameState {
    RUNNING,
    PAUSED,
    GAME_OVER,
    EXIT
};

// 带有生存时间的食物结构体
struct FoodWithLifetime {
    Food food;
    std::chrono::time_point<std::chrono::steady_clock> expirationTime;
    bool isExpired() const {
        return std::chrono::steady_clock::now() >= expirationTime;
    }
};

// 游戏类
class Game {
private:
    // 游戏状态
    std::atomic<GameState> state;
    
    // 游戏地图
    Map map;
    
    // 蛇
    Snake snake;
    
    // 食物列表
    std::vector<FoodWithLifetime> foods;
    
    // 最大同时存在的食物数量
    const size_t maxFoods = 5;
    
    // 食物生存时间（秒）
    const unsigned int foodLifetime = 15;
    
    // 显示接口
    Display display;
    
    // 输入处理
    Input input;
    
    // 游戏线程
    std::thread gameThread;
    std::thread renderThread;
    std::thread inputThread;
    
    // 线程同步
    std::mutex gameMutex;
    
    // 屏幕尺寸
    int screenWidth;
    int screenHeight;
    
    // 单元格大小
    int cellSize;
    
    // 游戏速度（毫秒/帧）
    int gameSpeed;
    
    // 原始游戏速度（用于辣椒效果结束后恢复）
    int originalGameSpeed;
    
    // 辣椒效果持续时间（毫秒）
    std::chrono::time_point<std::chrono::steady_clock> pepperEffectEndTime;
    
    // 辣椒效果是否激活
    bool pepperEffectActive;
    
    // 资源路径
    std::string resourcePath;
    
    bool isGameOverDrawn = false;

    // 游戏主循环
    void gameLoop();
    
    // 渲染循环
    void renderLoop();
    
    // 输入处理循环
    void inputLoop();
    
    // 更新游戏状态
    void update();
    
    // 处理碰撞
    void handleCollisions();
    
    // 检查辣椒效果是否结束
    void checkPepperEffect();
    
    // 生成食物
    void generateFood();
    
    // 检查和移除过期食物
    void checkAndRemoveExpiredFoods();
    
public:
    // 构造函数
    Game(int width, int height, int cellSize = 40, const std::string& resourcePath = "./assets/pic");
    
    // 析构函数
    ~Game();
    
    // 初始化游戏
    bool initialize();
    
    // 开始游戏
    void start();
    
    // 暂停游戏
    void pause();
    
    // 继续游戏
    void resume();
    
    // 结束游戏
    void exit();
    
    // 重置游戏
    void reset();
    
    // 等待所有线程结束
    void waitForThreads();
    
    // 获取当前游戏状态
    GameState getState() const;
    
    void updateMap();
};

#endif // GAME_H