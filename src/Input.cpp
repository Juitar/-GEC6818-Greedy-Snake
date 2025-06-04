#include "../include/Input.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <linux/input.h>
#include <cmath>

// 构造函数
Input::Input(const std::string& devicePath) : 
    touchFd(-1), 
    initialized(false),
    lastTouchX(0),
    lastTouchY(0),
    devicePath(devicePath) {
}

// 析构函数
Input::~Input() {
    close();
}

// 初始化输入设备
bool Input::initialize() {
    if (initialized) {
        return true;
    }
    
    // 打开触摸屏设备文件
    touchFd = open(devicePath.c_str(), O_RDONLY);
    if (touchFd == -1) {
        std::cerr << "Error opening touch device: " << strerror(errno) << std::endl;
        return false;
    }
    
    initialized = true;
    std::cout << "Touch device initialized successfully!" << std::endl;
    return true;
}

// 获取输入事件
bool Input::getEvent(InputEvent& event) {
    if (!initialized) {
        return false;
    }
    
    struct input_event ev;
    ssize_t res;
    
    // 非阻塞方式读取触摸屏数据
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(touchFd, &fds);
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    if (select(touchFd + 1, &fds, NULL, NULL, &tv) > 0) {
        res = read(touchFd, &ev, sizeof(ev));
        if (res == -1) {
            std::cerr << "Error reading touch device: " << strerror(errno) << std::endl;
            return false;
        } else if (res != sizeof(ev)) {
            std::cerr << "Incomplete read from touch device" << std::endl;
            return false;
        }
        
        static int x = 0, y = 0;
        static int x0 = 0, y0 = 0;
        
        // 分析触摸屏数据
        if (ev.type == EV_ABS) {
            // 绝对值事件类型
            if (ev.code == ABS_X) {
                // 触摸点X值的事件
                x = ev.value;
                // 如果是GEC6818屏幕，可能需要进行等比缩放
                x = x * 799 / 1023.0;
            } else if (ev.code == ABS_Y) {
                // 触摸点Y值的事件
                y = ev.value;
                // 如果是GEC6818屏幕，可能需要进行等比缩放
                y = y * 479 / 599.0;
            }
        } else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            // 触摸屏按键事件类型(按下和释放)
            if (ev.value == 1) {
                // 触摸屏被按下(手指接触触摸屏)
                // 获取起始点坐标
                x0 = x;
                y0 = y;
                event.type = InputEventType::TOUCH_DOWN;
                event.x = x;
                event.y = y;
                return true;
            } else if (ev.value == 0) {
                // 触摸屏被释放(手指离开触摸屏)
                // 判断滑动方向
                if (std::abs(x - x0) >= std::abs(y - y0) && std::abs(x - x0) >= 30) {
                    // 水平方向滑动
                    if (x > x0) {
                        // 向右滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.x = x;
                        event.y = y;
                        event.direction = Direction::RIGHT;
                        return true;
                    } else {
                        // 向左滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.x = x;
                        event.y = y;
                        event.direction = Direction::LEFT;
                        return true;
                    }
                } else if (std::abs(x - x0) < std::abs(y - y0) && std::abs(y - y0) >= 30) {
                    // 垂直方向滑动
                    if (y > y0) {
                        // 向下滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.x = x;
                        event.y = y;
                        event.direction = Direction::DOWN;
                        return true;
                    } else {
                        // 向上滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.x = x;
                        event.y = y;
                        event.direction = Direction::UP;
                        return true;
                    }
                } else {
                    // 点击事件
                    event.type = InputEventType::TOUCH_UP;
                    event.x = x;
                    event.y = y;
                    return true;
                }
            }
        }
    }
    return false;
}

// 将输入事件转换为蛇的方向
Direction Input::convertEventToDirection(const InputEvent& event, Direction currentDirection) {
    // 如果是移动事件，直接返回事件中的方向
    if (event.type == InputEventType::TOUCH_MOVE) {
        // 防止蛇直接掉头（这会导致蛇撞到自己）
        if ((currentDirection == Direction::UP && event.direction == Direction::DOWN) ||
            (currentDirection == Direction::DOWN && event.direction == Direction::UP) ||
            (currentDirection == Direction::LEFT && event.direction == Direction::RIGHT) ||
            (currentDirection == Direction::RIGHT && event.direction == Direction::LEFT)) {
            return currentDirection;
        }
        return event.direction;
    }
    
    // 其他情况保持当前方向
    return currentDirection;
}

// 关闭输入设备
void Input::close() {
    if (initialized && touchFd != -1) {
        ::close(touchFd);
        touchFd = -1;
        initialized = false;
        std::cout << "Touch device closed!" << std::endl;
    }
} 