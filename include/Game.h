#ifndef GAME_H
#define GAME_H

#include "Snake.h"
#include "Food.h"
#include "Map.h"
#include "Display.h"
#include "Input.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

// 游戏状态枚举
enum class GameState {
    RUNNING,
    PAUSED,
    GAME_OVER,
    EXIT
};

// 游戏类
class Game {
private:
    // 蛇对象
    Snake* snake;
    // 食物对象
    Food* food;
    // 地图对象
    Map* map;
    // 显示对象
    Display* display;
    // 输入对象
    Input* input;
    // 游戏状态
    GameState state;
    // 游戏分数
    int score;
    // 游戏速度（决定蛇移动的频率）
    int speed;
    // 游戏难度
    int difficulty;
    
    // 线程同步相关
    std::mutex gameMutex;
    std::condition_variable gameCondVar;
    std::atomic<bool> gameRunning;
    
    // 线程对象
    std::thread* inputThread;     // 处理输入事件
    std::thread* gameLoopThread;  // 游戏主循环
    std::thread* renderThread;    // 渲染线程
    std::thread* foodThread;      // 食物生成线程
    
    // 更新地图上的元素
    void updateMap();
    
    // 线程函数
    void inputLoop();        // 输入处理循环
    void gameLoop();         // 游戏主循环
    void renderLoop();       // 渲染循环
    void foodManagementLoop(); // 食物管理循环

public:
    // 构造函数
    Game(int mapWidth, int mapHeight);
    
    // 析构函数
    ~Game();
    
    // 初始化游戏
    void initialize();
    
    // 开始游戏
    void start();
    
    // 暂停游戏
    void pause();
    
    // 恢复游戏
    void resume();
    
    // 结束游戏
    void end();
    
    // 处理输入
    void handleInput(Direction direction);
    
    // 更新游戏状态
    void update();
    
    // 渲染游戏画面（预留接口，后续接入图像显示）
    void render();
    
    // 获取游戏状态
    GameState getState() const;
    
    // 获取游戏分数
    int getScore() const;
    
    // 设置游戏难度
    void setDifficulty(int difficulty);
    
    // 重置游戏
    void reset();
    
    // 等待所有线程结束
    void waitForThreads();
    
    // 静态变量，用于在线程间共享游戏状态
    static std::atomic<bool> isGameOver;
};

#endif // GAME_H 