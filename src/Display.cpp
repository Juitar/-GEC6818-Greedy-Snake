#include "../include/Display.h"
#include "../include/Game.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <chrono>

// 构造函数
Display::Display(int width, int height, int cellSize)
    : screenWidth(width),
      screenHeight(height),
      cellSize(cellSize),
      fbFd(-1),
      fbp(nullptr),
      screenSize(0),
      resourcePath(""),
      resourcesLoaded(false),
      backgroundDrawn(false) {
}

// 析构函数
Display::~Display() {
    close();
}

// 初始化显示
bool Display::initialize() {
    // 打开帧缓冲设备
    fbFd = open("/dev/fb0", O_RDWR);
    if (fbFd == -1) {
        std::cerr << "Error opening framebuffer device" << std::endl;
        return false;
    }
    
    // 获取屏幕固定信息
    if (ioctl(fbFd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        std::cerr << "Error reading fixed screen info" << std::endl;
        close();
        return false;
    }
    
    // 获取屏幕可变信息
    if (ioctl(fbFd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        std::cerr << "Error reading variable screen info" << std::endl;
        close();
        return false;
    }
    
    // 计算屏幕大小
    screenSize = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
    
    // 映射帧缓冲区
    fbp = (char*)mmap(0, screenSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbFd, 0);
    if ((long)fbp == -1) {
        std::cerr << "Error mapping framebuffer device to memory" << std::endl;
        close();
        return false;
    }
    
    // 设置屏幕尺寸
    screenWidth = vinfo.xres;
    screenHeight = vinfo.yres;
    
    // 创建背景缓冲区（减少频闪）
    try {
        bgBuffer = new char[screenSize];
        if (!bgBuffer) {
            std::cerr << "Failed to allocate background buffer" << std::endl;
        } else {
            std::cout << "Background buffer created" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to create background buffer: " << e.what() << std::endl;
        bgBuffer = nullptr;
    }
    
    std::cout << "Framebuffer initialized: " << screenWidth << "x" << screenHeight 
              << ", " << vinfo.bits_per_pixel << " bpp" << std::endl;
    
    return true;
}

// 清空屏幕
void Display::clear() {
    if (fbp) {
        // 用黑色填充整个屏幕
        std::memset(fbp, 0, screenSize);
    }
}

// 加载BMP资源
bool Display::loadResources(const std::string& path) {
    try {
        // 检查路径是否为空
        if (path.empty()) {
            std::cerr << "Error: Resource path is empty" << std::endl;
            return false;
        }
        
        // 检查路径是否存在
        if (access(path.c_str(), F_OK) == -1) {
            std::cerr << "Error: Resource path does not exist: " << path << std::endl;
            return false;
        }
        
        resourcePath = path;
        std::cout << "Loading resources from: " << resourcePath << std::endl;
        
        // 构建各种游戏元素的BMP路径
        // 蛇头图片（四个方向）
        snakeHeadBmpUp = resourcePath + "/head_up.bmp";
        snakeHeadBmpDown = resourcePath + "/head_down.bmp";
        snakeHeadBmpLeft = resourcePath + "/head_left.bmp";
        snakeHeadBmpRight = resourcePath + "/head_right.bmp";
        
        // 蛇身图片（垂直和水平）
        snakeBodyBmpVertical = resourcePath + "/body_up&down.bmp";
        snakeBodyBmpHorizontal = resourcePath + "/body_left&right.bmp";
        
        // 蛇身拐角图片
        snakeBodyBmpUL = resourcePath + "/body_UL.bmp";
        snakeBodyBmpUR = resourcePath + "/body_UR.bmp";
        snakeBodyBmpDL = resourcePath + "/body_DL.bmp";
        snakeBodyBmpDR = resourcePath + "/body_DR.bmp";
        
        // 蛇尾图片（四个方向）
        snakeTailBmpUp = resourcePath + "/tail_up.bmp";
        snakeTailBmpDown = resourcePath + "/tail_down.bmp";
        snakeTailBmpLeft = resourcePath + "/tail_left.bmp";
        snakeTailBmpRight = resourcePath + "/tail_right.bmp";
        
        // 食物图片
        appleBmp = resourcePath + "/apple.bmp";
        pepperBmp = resourcePath + "/pepper.bmp";
        meatBmp = resourcePath + "/meat.bmp";
        bombBmp = resourcePath + "/bomb.bmp";
        
        // 兼容旧版本，将apple.bmp设为默认食物图片
        foodBmp = appleBmp;
        

        // 背景图片
        grass1Bmp = resourcePath + "/grass1.bmp";
        grass2Bmp = resourcePath + "/grass2.bmp";
        
        //game_over.bmp
        game_overBmp = resourcePath + "/game_over.bmp";
        
        // 输出调试信息
        std::cout << "Checking resources..." << std::endl;
        std::cout << "Resource path: " << resourcePath << std::endl;
        
        // 检查必要文件是否存在
        std::vector<std::pair<std::string, std::string>> requiredFiles = {
            {snakeHeadBmpUp, "Snake head up"},
            {snakeHeadBmpDown, "Snake head down"},
            {snakeHeadBmpLeft, "Snake head left"},
            {snakeHeadBmpRight, "Snake head right"},
            {snakeBodyBmpVertical, "Snake body vertical"},
            {snakeBodyBmpHorizontal, "Snake body horizontal"},
            {appleBmp, "Apple"},
            {grass1Bmp, "Grass 1"},
            {grass2Bmp, "Grass 2"}
        };
        
        // 检查每个必要文件
        for (const auto& file : requiredFiles) {
            std::cout << "Checking file: " << file.first << std::endl;
            if (access(file.first.c_str(), F_OK) == -1) {
                std::cerr << "Error: Required file not found: " << file.first << " (" << file.second << ")" << std::endl;
                return false;
            } else {
                std::cout << "Found: " << file.second << " at " << file.first << std::endl;
            }
        }
        
        // 检查可选文件（拐角、尾部和其他食物）
        std::vector<std::pair<std::string, std::string>> optionalFiles = {
            {snakeBodyBmpUL, "Snake body UL corner"},
            {snakeBodyBmpUR, "Snake body UR corner"},
            {snakeBodyBmpDL, "Snake body DL corner"},
            {snakeBodyBmpDR, "Snake body DR corner"},
            {snakeTailBmpUp, "Snake tail up"},
            {snakeTailBmpDown, "Snake tail down"},
            {snakeTailBmpLeft, "Snake tail left"},
            {snakeTailBmpRight, "Snake tail right"},
            {pepperBmp, "Pepper"},
            {meatBmp, "Meat"},
            {bombBmp, "Bomb"}
        };
        
        // 检查每个可选文件
        for (const auto& file : optionalFiles) {
            if (access(file.first.c_str(), F_OK) == -1) {
                std::cout << "Warning: Optional file not found: " << file.first << " (" << file.second << ")" << std::endl;
            } else {
                std::cout << "Found: " << file.second << " at " << file.first << std::endl;
            }
        }
        
        resourcesLoaded = true;
        std::cout << "All required resources loaded successfully!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading resources: " << e.what() << std::endl;
        return false;
    }
}

// 绘制地图
void Display::drawMap(const Map* map) {
    if (!fbp || !resourcesLoaded) return;
    
    // 如果背景已经绘制过，并且有背景缓冲区，则直接复制背景
    if (backgroundDrawn && bgBuffer) {
        // 复制预渲染的背景到帧缓冲区
        std::memcpy(fbp, bgBuffer, screenSize);
        return;
    }
    
    // 首次绘制背景
    // 绘制背景（棋盘形式的草地）
    int mapWidth = screenWidth / cellSize;
    int mapHeight = screenHeight / cellSize;
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // 计算屏幕坐标
            int screenX = x * cellSize;
            int screenY = y * cellSize;
            
            // 棋盘式交替绘制grass1和grass2
            if ((x + y) % 2 == 0) {
                drawBmp(screenX, screenY, grass1Bmp);
            } else {
                drawBmp(screenX, screenY, grass2Bmp);
            }
        }
    }
    
    // 保存背景到缓冲区（如果有）
    if (bgBuffer) {
        std::memcpy(bgBuffer, fbp, screenSize);
        backgroundDrawn = true;
        std::cout << "Background cached to reduce flicker" << std::endl;
    }
    
    // 如果有地图对象，绘制地图元素
    if (map) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
        for (int y = 0; y < map->getHeight(); y++) {
            for (int x = 0; x < map->getWidth(); x++) {
                MapElementType element = map->getElement(x, y);
                
                // 计算屏幕坐标
                int screenX = x * cellSize;
                int screenY = y * cellSize;
                
                // 根据元素类型绘制不同的图像
                switch (element) {
                    case MapElementType::WALL:
                        // 墙壁暂时不绘制
                        break;
                    case MapElementType::FOOD:
                        // 食物在drawFood中绘制
                        break;
                    case MapElementType::SNAKE_HEAD:
                    case MapElementType::SNAKE_BODY:
                        // 蛇在drawSnake中绘制
                        break;
                    case MapElementType::EMPTY:
                    default:
                        // 空白区域已经绘制了草地背景
                        break;
                }
            }
        }
#pragma GCC diagnostic pop
    }
}

// 绘制蛇
void Display::drawSnake(const Snake* snake) {
    if (!fbp || !snake || !resourcesLoaded) return;
    
    // 获取蛇身体
    const auto& body = snake->getBody();
    
    if (body.empty()) return;
    
    try {
        // 绘制蛇头，根据方向选择正确的图片
        int headX = body[0].first * cellSize;
        int headY = body[0].second * cellSize;
        
        Direction snakeDirection = snake->getDirection();
        switch (snakeDirection) {
            case Direction::UP:
                if (access(snakeHeadBmpUp.c_str(), F_OK) != -1) {
                    drawTransparentBmp(headX, headY, snakeHeadBmpUp);
                } else {
                    // 如果特定方向的图片不存在，使用默认图片
                    drawTransparentBmp(headX, headY, snakeHeadBmpRight);
                }
                break;
            case Direction::DOWN:
                if (access(snakeHeadBmpDown.c_str(), F_OK) != -1) {
                    drawTransparentBmp(headX, headY, snakeHeadBmpDown);
                } else {
                    drawTransparentBmp(headX, headY, snakeHeadBmpRight);
                }
                break;
            case Direction::LEFT:
                if (access(snakeHeadBmpRight.c_str(), F_OK) != -1) {
                    drawTransparentBmp(headX, headY, snakeHeadBmpRight);
                } else {
                    drawTransparentBmp(headX, headY, snakeHeadBmpRight);
                }
                break;
            case Direction::RIGHT:
            default:
                if (access(snakeHeadBmpLeft.c_str(), F_OK) != -1) {
                    drawTransparentBmp(headX, headY, snakeHeadBmpLeft);
                } else {
                    drawTransparentBmp(headX, headY, snakeHeadBmpRight);
                }
                break;
        }
        
        // 如果蛇身长度大于1，绘制蛇身
        if (body.size() > 1) {
            // 绘制蛇身，根据相邻节点位置确定方向
            for (std::size_t i = 1; i < body.size() - 1 && i < body.size(); i++) {
                int bodyX = body[i].first * cellSize;
                int bodyY = body[i].second * cellSize;
                
                // 获取前一个和后一个节点的位置
                std::pair<int, int> prev = body[i-1];
                std::pair<int, int> curr = body[i];
                std::pair<int, int> next = body[i+1];
                
                // 确定身体部分的方向
                if (prev.first == next.first) {
                    // 垂直方向
                    drawTransparentBmp(bodyX, bodyY, snakeBodyBmpVertical);
                } else if (prev.second == next.second) {
                    // 水平方向
                    drawTransparentBmp(bodyX, bodyY, snakeBodyBmpHorizontal);
                } else {
                    // 拐角，根据前后节点位置确定拐角类型
                    std::string cornerBmp;
                    
                    // 修正拐角判断逻辑，确保方向正确
                    if ((prev.first < curr.first && next.second < curr.second) || 
                        (prev.second < curr.second && next.first < curr.first)) {
                        // 左上拐角
                        cornerBmp = snakeBodyBmpUL;
                    } else if ((prev.first > curr.first && next.second < curr.second) || 
                              (prev.second < curr.second && next.first > curr.first)) {
                        // 右上拐角
                        cornerBmp = snakeBodyBmpUR;
                    } else if ((prev.first < curr.first && next.second > curr.second) || 
                              (prev.second > curr.second && next.first < curr.first)) {
                        // 左下拐角
                        cornerBmp = snakeBodyBmpDL;
                    } else {
                        // 右下拐角
                        cornerBmp = snakeBodyBmpDR;
                    }
                    
                    // 检查拐角图片是否存在
                    if (access(cornerBmp.c_str(), F_OK) == -1) {
                        // 如果拐角图片不存在，使用默认的身体图片
                        if (prev.first == curr.first || next.first == curr.first) {
                            drawTransparentBmp(bodyX, bodyY, snakeBodyBmpVertical);
                        } else {
                            drawTransparentBmp(bodyX, bodyY, snakeBodyBmpHorizontal);
                        }
                    } else {
                        drawTransparentBmp(bodyX, bodyY, cornerBmp);
                    }
                }
            }
            
            // 绘制蛇尾，根据倒数第二个节点的位置确定方向
            if (body.size() >= 2) {
                int tailX = body.back().first * cellSize;
                int tailY = body.back().second * cellSize;
                
                std::pair<int, int> tailPart = body.back();
                std::pair<int, int> beforeTail = body[body.size() - 2];
                
                std::string tailBmp;
                // 修正尾部方向判断逻辑
                if (beforeTail.first == tailPart.first) {
                    // 垂直方向
                    if (beforeTail.second < tailPart.second) {
                        // 尾部在下，头部在上方向
                        tailBmp = snakeTailBmpUp;
                    } else {
                        // 尾部在上，头部在下方向
                        tailBmp = snakeTailBmpDown;
                    }
                } else {
                    // 水平方向
                    if (beforeTail.first < tailPart.first) {
                        // 尾部在右，头部在左方向
                        tailBmp = snakeTailBmpLeft;
                    } else {
                        // 尾部在左，头部在右方向
                        tailBmp = snakeTailBmpRight;
                    }
                }
                
                // 检查尾部图片是否存在
                if (access(tailBmp.c_str(), F_OK) == -1) {
                    // 如果尾部图片不存在，使用默认的身体图片
                    if (beforeTail.first == tailPart.first) {
                        drawTransparentBmp(tailX, tailY, snakeBodyBmpVertical);
                    } else {
                        drawTransparentBmp(tailX, tailY, snakeBodyBmpHorizontal);
                    }
                } else {
                    drawTransparentBmp(tailX, tailY, tailBmp);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error drawing snake: " << e.what() << std::endl;
    }
}

// 绘制食物
void Display::drawFood(const Food* food) {
    if (!fbp || !food || !resourcesLoaded) return;
    
    // 获取食物位置
    int foodX = food->getX() * cellSize;
    int foodY = food->getY() * cellSize;
    
    // 根据食物类型选择不同的图片
    std::string foodImage;
    switch (food->getType()) {
        case FoodType::APPLE:
            foodImage = appleBmp;
            break;
        case FoodType::PEPPER:
            // 如果辣椒图片不存在，使用苹果图片
            if (access(pepperBmp.c_str(), F_OK) != -1) {
                foodImage = pepperBmp;
            } else {
                foodImage = appleBmp;
            }
            break;
        case FoodType::MEAT:
            // 如果肉图片不存在，使用苹果图片
            if (access(meatBmp.c_str(), F_OK) != -1) {
                foodImage = meatBmp;
            } else {
                foodImage = appleBmp;
            }
            break;
        case FoodType::BOMB:
            // 如果炸弹图片不存在，使用苹果图片
            if (access(bombBmp.c_str(), F_OK) != -1) {
                foodImage = bombBmp;
            } else {
                foodImage = appleBmp;
            }
            break;
        default:
            foodImage = appleBmp;
            break;
    }
    
    // 绘制食物图像，使用透明背景
    drawTransparentBmp(foodX, foodY, foodImage);
}


// 绘制分数
void Display::drawScore(int score) {
    // 由于不需要分数系统，此方法可以保留但不使用
}

// 绘制游戏状态信息
void Display::drawGameState(GameState state) {
    if (!fbp) return;
    
    std::string stateStr;
    
    switch (state) {
        case GameState::RUNNING:
            stateStr = "Running";
            break;
        case GameState::PAUSED:
            stateStr = "Paused";
            break;
        case GameState::GAME_OVER:
            stateStr = "Game Over";
            break;
        case GameState::EXIT:
            stateStr = "Exiting";
            break;
    }
    
    drawBmp(12, 20, stateBmp);
    
    // 这里应该使用字体渲染库绘制文字
    // 由于没有字体渲染库，这里只是简单地在控制台输出状态
    std::cout << "Game State: " << stateStr << std::endl;
}


// 更新屏幕
void Display::update() {
    // 帧缓冲区直接映射到显示器，不需要额外的更新操作
}

// 关闭显示
void Display::close() {
    if (fbp) {
        munmap(fbp, screenSize);
        fbp = nullptr;
    }
    
    if (fbFd != -1) {
        ::close(fbFd);
        fbFd = -1;
    }
    
    // 释放背景缓冲区
    if (bgBuffer) {
        delete[] bgBuffer;
        bgBuffer = nullptr;
    }
}

// 绘制BMP图像
void Display::drawBmp(int x, int y, const std::string& bmpPath) {
    if (!fbp) return;
    
    try {
        // 检查文件是否存在
        if (access(bmpPath.c_str(), F_OK) == -1) {
            std::cerr << "Warning: BMP file not found: " << bmpPath << std::endl;
            return;
        }
        
        // 输出调试信息
        // std::cout << "Drawing BMP: " << bmpPath << " at (" << x << ", " << y << ")" << std::endl;
        
        // 使用lcd_draw_bmp绘制BMP图像
        lcd_draw_bmp(fbp, &vinfo, x, y, bmpPath.c_str());
    } catch (const std::exception& e) {
        std::cerr << "Error drawing BMP: " << e.what() << " (file: " << bmpPath << ")" << std::endl;
    }
}

// 绘制透明背景的BMP图像
void Display::drawTransparentBmp(int x, int y, const std::string& bmpPath, unsigned int transparentColor) {
    if (!fbp) return;
    
    try {
        // 检查文件是否存在
        if (access(bmpPath.c_str(), F_OK) == -1) {
            std::cerr << "Warning: BMP file not found: " << bmpPath << std::endl;
            return;
        }
        
        // 输出调试信息
        // std::cout << "Drawing transparent BMP: " << bmpPath << " at (" << x << ", " << y << ")" << std::endl;
        
        // 使用lcd_draw_bmp_transparent绘制带透明背景的BMP图像
        lcd_draw_bmp_transparent(fbp, &vinfo, x, y, bmpPath.c_str(), transparentColor);
    } catch (const std::exception& e) {
        std::cerr << "Error drawing transparent BMP: " << e.what() << " (file: " << bmpPath << ")" << std::endl;
    }
}

// 绘制一个像素点
void Display::drawPoint(int x, int y, unsigned int color) {
    if (!fbp) return;
    
    // 使用lcd_draw_point绘制像素点
    lcd_draw_point(fbp, &vinfo, x, y, color);
}

bool Display::getBmpSize(const std::string& filePath, int* width, int* height) {
    FILE* fp = fopen(filePath.c_str(), "rb");
    if (!fp) return false;

    unsigned char header[54];
    if (fread(header, 1, 54, fp) != 54) {
        fclose(fp);
        return false;
    }

    *width = *(int*)&header[18];
    *height = *(int*)&header[22];

    fclose(fp);
    return true;
}

void Display::drawGameOver() {
    if (!fbp) return;

    int bmpWidth = 0, bmpHeight = 0;
    if (!getBmpSize(game_overBmp, &bmpWidth, &bmpHeight)) {
        std::cerr << "[Display] Failed to read BMP dimensions." << std::endl;
        return;
    }

    int x = (screenWidth - bmpWidth) / 2;
    int y = (screenHeight - bmpHeight) / 2;

    
    unsigned int transparentColor = 0xFFFFFFFF; 

    drawTransparentBmp(x, y, game_overBmp, transparentColor);

    update();
}