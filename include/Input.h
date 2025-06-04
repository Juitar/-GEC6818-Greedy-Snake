#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "Snake.h"

// 输入事件类型
enum class InputEventType {
    NONE,
    TOUCH_DOWN,
    TOUCH_UP,
    TOUCH_MOVE,
    KEY_PRESS,
    KEY_RELEASE
};

// 输入事件结构体
struct InputEvent {
    InputEventType type;
    int x;
    int y;
    Direction direction;  // 用于TOUCH_MOVE事件
    int keyCode;          // 用于键盘事件（预留）
    
    InputEvent() : type(InputEventType::NONE), x(0), y(0), direction(Direction::RIGHT), keyCode(0) {}
};

// 输入接口类
class Input {
private:
    // 输入设备文件描述符
    int touchFd;
    // 是否初始化成功
    bool initialized;
    // 上一次触摸位置
    int lastTouchX;
    int lastTouchY;
    // 设备路径
    std::string devicePath;
    
    // 事件队列，存储待处理的输入事件
    std::queue<InputEvent> eventQueue;
    // 互斥锁，保护事件队列
    std::mutex eventMutex;
    // 条件变量，用于通知有新事件
    std::condition_variable hasEvent;
    // 当前方向
    std::atomic<Direction> currentDirection;
    std::mutex directionMutex;
    bool newInput;
    
    // 读取设备数据的线程函数
    void readDeviceThread();
    // 检查设备是否就绪的函数
    bool isDeviceReady();

public:
    // 构造函数
    Input();
    
    // 析构函数
    ~Input();
    
    // 初始化输入设备
    bool initialize();
    
    // 获取输入事件
    bool getEvent(InputEvent& event);
    
    // 将输入事件转换为蛇的方向
    Direction convertEventToDirection(const InputEvent& event, Direction currentDirection);
    
    // 关闭输入设备
    void close();
    
    // 检查输入设备是否已初始化
    bool isInitialized() const { return initialized; }
    
    // 获取当前方向
    Direction getDirection() const;
    
    // 检查是否有新的输入
    bool hasNewInput() const;
    
    // 清除新输入标记
    void clearNewInput();
    
    // 添加事件到队列
    void addEvent(const InputEvent& event);
    
    // 处理键盘输入
    void processKeyboardInput();
    
    // 启动输入线程
    std::thread* startInputThread();
    
    // 设置当前方向
    void setDirection(Direction dir);
};

#endif // INPUT_H 