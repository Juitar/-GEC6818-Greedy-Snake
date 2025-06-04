#include "../include/Input.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <linux/input.h>
#include <cmath>
#include <poll.h>
#include <thread>
#include <mutex>

// 构造函数
Input::Input() : currentDirection(Direction::RIGHT), newInput(false), touchFd(-1), initialized(false) {
}

// 析构函数
Input::~Input() {
    close();
}

// 初始化输入设备
bool Input::initialize() {
    // 尝试打开触摸屏设备
    touchFd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (touchFd == -1) {
        std::cerr << "Warning: Could not open touch device, falling back to keyboard input" << std::endl;
    } else {
        std::cout << "Touch device initialized successfully!" << std::endl;
    }
    
    initialized = true;
    return true;
}

// 检查设备是否就绪
bool Input::isDeviceReady() {
    if (touchFd == -1) {
        return false;
    }
    
    struct pollfd pfd;
    pfd.fd = touchFd;
    pfd.events = POLLIN;
    
    // 轮询设备，超时时间为0（非阻塞）
    int ret = poll(&pfd, 1, 0);
    
    return (ret > 0 && (pfd.revents & POLLIN));
}

// 读取设备数据的线程函数
void Input::readDeviceThread() {
    struct input_event ev;
    ssize_t res;
    int x = 0, y = 0;
    int x0 = 0, y0 = 0;
    
    while (touchFd != -1) {
        // 检查设备是否就绪
        if (!isDeviceReady()) {
            // 如果设备没有数据，短暂休眠减少CPU占用
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // 读取触摸屏数据
        res = read(touchFd, &ev, sizeof(ev));
        if (res == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 非阻塞模式下，没有数据可读
                continue;
            }
            std::cerr << "Error reading touch device: " << strerror(errno) << std::endl;
            continue;
        } else if (res != sizeof(ev)) {
            std::cerr << "Incomplete read from touch device" << std::endl;
            continue;
        }
        
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
                
                // 创建按下事件
                InputEvent event;
                event.type = InputEventType::TOUCH_DOWN;
                event.x = x;
                event.y = y;
                addEvent(event);
                
            } else if (ev.value == 0) {
                // 触摸屏被释放(手指离开触摸屏)
                // 创建释放事件
                InputEvent event;
                event.type = InputEventType::TOUCH_UP;
                event.x = x;
                event.y = y;
                
                // 判断滑动方向
                if (std::abs(x - x0) >= std::abs(y - y0) && std::abs(x - x0) >= 30) {
                    // 水平方向滑动
                    if (x > x0) {
                        // 向右滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.direction = Direction::RIGHT;
                    } else {
                        // 向左滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.direction = Direction::LEFT;
                    }
                } else if (std::abs(x - x0) < std::abs(y - y0) && std::abs(y - y0) >= 30) {
                    // 垂直方向滑动
                    if (y > y0) {
                        // 向下滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.direction = Direction::DOWN;
                    } else {
                        // 向上滑
                        event.type = InputEventType::TOUCH_MOVE;
                        event.direction = Direction::UP;
                    }
                }
                
                // 添加事件到队列
                if (event.type == InputEventType::TOUCH_MOVE) {
                    addEvent(event);
                    
                    // 立即更新方向，提高响应速度
                    Direction newDirection = convertEventToDirection(event, currentDirection);
                    if (newDirection != currentDirection) {
                        currentDirection = newDirection;
                        newInput = true;
                    }
                }
            }
        }
    }
}

// 添加事件到队列
void Input::addEvent(const InputEvent& event) {
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push(event);
    }
    hasEvent.notify_one();
}

// 获取输入事件
bool Input::getEvent(InputEvent& event) {
    if (touchFd == -1) {
        return false;
    }
    
    std::unique_lock<std::mutex> lock(eventMutex);
    if (eventQueue.empty()) {
        // 非阻塞模式，队列为空时立即返回
        return false;
    }
    
    event = eventQueue.front();
    eventQueue.pop();
    return true;
}

// 处理键盘输入
void Input::processKeyboardInput() {
    // 非阻塞方式读取键盘输入
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    
    int ret = poll(&pfd, 1, 0);
    if (ret > 0 && (pfd.revents & POLLIN)) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            Direction newDir = currentDirection;
            
            // 根据输入的键值确定方向
            switch (c) {
                case 'w': // 上
                    newDir = Direction::UP;
                    break;
                case 's': // 下
                    newDir = Direction::DOWN;
                    break;
                case 'a': // 左
                    newDir = Direction::LEFT;
                    break;
                case 'd': // 右
                    newDir = Direction::RIGHT;
                    break;
                default:
                    // 其他键不处理
                    return;
            }
            
            // 设置新方向
            setDirection(newDir);
        }
    }
}

// 将输入事件转换为蛇的方向
Direction Input::convertEventToDirection(const InputEvent& event, Direction currentDirection) {
    Direction newDirection = currentDirection;
    
    // 根据事件类型处理
    if (event.type == InputEventType::TOUCH_MOVE) {
        newDirection = event.direction;
    } else if (event.type == InputEventType::KEY_PRESS) {
        newDirection = event.direction;
    }
    
    // 防止蛇直接掉头（这会导致蛇撞到自己）
    if ((currentDirection == Direction::UP && newDirection == Direction::DOWN) ||
        (currentDirection == Direction::DOWN && newDirection == Direction::UP) ||
        (currentDirection == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (currentDirection == Direction::RIGHT && newDirection == Direction::LEFT)) {
        return currentDirection;
    }
    
    return newDirection;
}

// 启动输入线程
std::thread* Input::startInputThread() {
    return new std::thread(&Input::readDeviceThread, this);
}

// 关闭输入设备
void Input::close() {
    if (touchFd != -1) {
        ::close(touchFd);
        touchFd = -1;
    }
}

// 设置当前方向
void Input::setDirection(Direction dir) {
    std::lock_guard<std::mutex> lock(directionMutex);
    currentDirection = dir;
    newInput = true;
}

// 获取当前方向
Direction Input::getDirection() const {
    return currentDirection;
}

// 检查是否有新的输入
bool Input::hasNewInput() const {
    return newInput;
}

// 清除新输入标记
void Input::clearNewInput() {
    std::lock_guard<std::mutex> lock(directionMutex);
    newInput = false;
} 