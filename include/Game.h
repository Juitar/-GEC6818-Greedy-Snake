#ifndef GAME_H
#define GAME_H

#include "Snake.h"
#include "Food.h"
#include "Map.h"

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
    // 游戏状态
    GameState state;
    // 游戏分数
    int score;
    // 游戏速度（决定蛇移动的频率）
    int speed;
    // 游戏难度
    int difficulty;
    
    // 更新地图上的元素
    void updateMap();

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
};

#endif // GAME_H 