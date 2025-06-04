#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "Map.h"
#include "Snake.h"
#include "Food.h"
#include "BmpDisplay.h"

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
    // 帧缓冲设备文件描述符
    int fbFd;
    // 帧缓冲区指针
    char* fbp;
    // 屏幕信息
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    // 屏幕缓冲区大小
    long int screenSize;
    // BMP资源路径
    std::string resourcePath;
    // BMP资源是否已加载
    bool resourcesLoaded;
    // 各种游戏元素的BMP路径
    // 蛇头图片（四个方向）
    std::string snakeHeadBmpUp;
    std::string snakeHeadBmpDown;
    std::string snakeHeadBmpLeft;
    std::string snakeHeadBmpRight;
    
    // 蛇身图片（垂直和水平）
    std::string snakeBodyBmpVertical;
    std::string snakeBodyBmpHorizontal;
    
    // 蛇身拐角图片
    std::string snakeBodyBmpUL;
    std::string snakeBodyBmpUR;
    std::string snakeBodyBmpDL;
    std::string snakeBodyBmpDR;
    
    // 蛇尾图片（四个方向）
    std::string snakeTailBmpUp;
    std::string snakeTailBmpDown;
    std::string snakeTailBmpLeft;
    std::string snakeTailBmpRight;
    
    // 食物图片
    std::string appleBmp;    // 苹果
    std::string pepperBmp;   // 辣椒
    std::string meatBmp;     // 肉
    std::string bombBmp;     // 炸弹
    
    std::string foodBmp;
    std::string grass1Bmp;
    std::string grass2Bmp;

    // 定义透明色（白色）
    static const unsigned int TRANSPARENT_COLOR = 0xFFFFFFFF;

    // 背景缓冲区（用于减少频闪）
    char* bgBuffer;
    
    // 背景是否已绘制的标志
    bool backgroundDrawn;

public:
    // 构造函数
    Display(int width, int height, int cellSize = 40);
    
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
    
    // 绘制BMP图像
    void drawBmp(int x, int y, const std::string& bmpPath);
    
    // 绘制透明背景的BMP图像
    void drawTransparentBmp(int x, int y, const std::string& bmpPath, unsigned int transparentColor = TRANSPARENT_COLOR);
    
    // 绘制一个像素点
    void drawPoint(int x, int y, unsigned int color);

    // 获取屏幕宽度
    int getScreenWidth() const { return screenWidth; }
    
    // 获取屏幕高度
    int getScreenHeight() const { return screenHeight; }
    
    // 获取单元格大小
    int getCellSize() const { return cellSize; }
};

#endif // DISPLAY_H 