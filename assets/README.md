# 贪吃蛇游戏资源目录

本目录存放贪吃蛇游戏所需的资源文件。

## 目录结构

- `/pic` - 包含游戏中使用的BMP图像文件
  - `grass1.bmp` - 草地背景图1
  - `grass2.bmp` - 草地背景图2
  - `head_up.bmp` - 蛇头（向上）
  - `head_down.bmp` - 蛇头（向下）
  - `head_left.bmp` - 蛇头（向左）
  - `head_right.bmp` - 蛇头（向右）
  - `body_up&down.bmp` - 蛇身（垂直）
  - `body_left&right.bmp` - 蛇身（水平）
  - `body_UL.bmp` - 蛇身（上左拐角）
  - `body_UR.bmp` - 蛇身（上右拐角）
  - `body_DL.bmp` - 蛇身（下左拐角）
  - `body_DR.bmp` - 蛇身（下右拐角）
  - `tail_up.bmp` - 蛇尾（向上）
  - `tail_down.bmp` - 蛇尾（向下）
  - `tail_left.bmp` - 蛇尾（向左）
  - `tail_right.bmp` - 蛇尾（向右）
  - `apple.bmp` - 食物（苹果）
  - `meat.bmp` - 食物（肉）
  - `pepper.bmp` - 食物（辣椒）
  - `bomb.bmp` - 障碍物（炸弹）

## 图像格式要求

所有图像都必须是BMP格式，尺寸为40x40像素，24位或32位色深。
白色（0xFFFFFFFF）将被视为透明色，在绘制时会被跳过。 