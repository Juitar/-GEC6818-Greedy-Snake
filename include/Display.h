#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include "Map.h"
#include "Snake.h"
#include "Food.h"

// 前向声明
enum class GameState;

// 显示接口类
class Display {
private:
    // 屏幕宽度
    int screenWidth;
    // 屏幕高度
    int screenHeight;
    // 单元格大小（像素）
    int cellSize;
    // 屏幕缓冲区
    void* screenBuffer;
    // BMP资源路径
    std::string resourcePath;

public:
    // 构造函数
    Display(int width, int height, int cellSize);
    
    // 析构函数
    ~Display();
    
    // 初始化显示
    bool initialize();
    
    // 清空屏幕
    void clear();
    
    // 加载BMP资源
    bool loadResources(const std::string& path);
    
    // 绘制地图
    void drawMap(const Map* map);
    
    // 绘制蛇
    void drawSnake(const Snake* snake);
    
    // 绘制食物
    void drawFood(const Food* food);
    
    // 绘制分数
    void drawScore(int score);
    
    // 绘制游戏状态信息
    void drawGameState(GameState state);
    
    // 更新屏幕
    void update();
    
    // 关闭显示
    void close();
};

#endif // DISPLAY_H 