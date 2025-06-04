#ifndef INPUT_H
#define INPUT_H

#include <string>
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

public:
    // 构造函数
    Input(const std::string& devicePath = "/dev/input/event0");
    
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
};

#endif // INPUT_H 