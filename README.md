# 贪吃蛇游戏 (Greedy Snake)

这是一个为S5P6818开发板开发的贪吃蛇游戏，使用C++编写。

## 项目结构

```
greedy-snake/
├── include/           # 头文件
│   ├── Snake.h        # 蛇类
│   ├── Food.h         # 食物类
│   ├── Map.h          # 地图类
│   ├── Game.h         # 游戏类
│   ├── Display.h      # 显示接口类
│   ├── Input.h        # 输入接口类
│   └── BmpDisplay.h   # BMP图像显示功能
├── src/               # 源代码
│   ├── Snake.cpp      # 蛇类实现
│   ├── Food.cpp       # 食物类实现
│   ├── Map.cpp        # 地图类实现
│   ├── Game.cpp       # 游戏类实现
│   ├── Input.cpp      # 输入类实现
│   ├── Display.cpp    # 显示类实现
│   ├── BmpDisplay.cpp # BMP图像显示功能实现
│   └── main.cpp       # 主程序
├── assets/            # 资源文件（图片等）
├── bin/               # 编译后的可执行文件
├── obj/               # 编译后的目标文件
└── Makefile           # 编译脚本
```

## 功能特点

- 基本的贪吃蛇游戏逻辑
- 多线程架构设计
- 触摸屏输入支持
- BMP图像显示支持
- 支持不同难度级别
- 支持分数系统

## 编译和运行

### 编译

```bash
make
```

### 运行

```bash
make run
```

或者直接运行可执行文件：

```bash
./bin/greedy-snake
```

## TODO 列表

以下是项目还需要完成的工作：

### 1. 图形显示实现
- [x] 实现Display类的具体功能
- [x] 添加BMP图像加载功能
- [x] 实现LCD屏幕驱动接口

### 2. 游戏功能扩展
- [x] 添加多种食物类型，具有不同效果
- [x] 实现特殊道具系统

### 3. 其他
- [x] 创建makefile脚本

## 开发环境要求

- GCC 编译器（支持C++11标准）
- S5P6818开发板
- 触摸屏设备
- Linux操作系统环境

## 注意事项

本项目是为S5P6818开发板设计的，如果要在其他平台上运行，可能需要修改显示和输入相关的代码。特别是触摸屏设备路径可能需要根据实际情况调整。

## 资源文件要求

游戏需要以下BMP格式的图像文件，放置在assets目录下：

- snake_head.bmp - 蛇头图像
- snake_body.bmp - 蛇身图像
- food.bmp - 食物图像
- wall.bmp - 墙体图像
- background.bmp - 背景图像

请确保这些文件存在且为24位或32位BMP格式。 