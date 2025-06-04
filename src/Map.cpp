#include "../include/Map.h"

// 构造函数
Map::Map(int width, int height) : width(width), height(height) {
    // 初始化地图网格
    grid.resize(height, std::vector<MapElementType>(width, MapElementType::EMPTY));
}

// 获取地图宽度
int Map::getWidth() const {
    return width;
}

// 获取地图高度
int Map::getHeight() const {
    return height;
}

// 获取指定位置的元素类型
MapElementType Map::getElement(int x, int y) const {
    // 检查坐标是否有效
    if (x < 0 || x >= width || y < 0 || y >= height) {
        // 如果坐标无效，返回墙壁类型（防止蛇越界）
        return MapElementType::WALL;
    }
    
    return grid[y][x];
}

// 设置指定位置的元素类型
void Map::setElement(int x, int y, MapElementType element) {
    // 检查坐标是否有效
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // 坐标无效，不执行操作
    }
    
    grid[y][x] = element;
}

// 清空地图
void Map::clear() {
    // 将所有元素设置为EMPTY
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid[y][x] = MapElementType::EMPTY;
        }
    }
}