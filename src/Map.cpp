#include "../include/Map.h"

// 构造函数
Map::Map(int width, int height) : width(width), height(height) {
    // 初始化地图数据
    data.resize(height, std::vector<MapElementType>(width, MapElementType::EMPTY));
}

// 初始化地图
void Map::initialize() {
    // 清空地图
    clear();
    
    // 设置地图边界为墙
    for (int i = 0; i < width; i++) {
        data[0][i] = MapElementType::WALL;
        data[height - 1][i] = MapElementType::WALL;
    }
    
    for (int i = 0; i < height; i++) {
        data[i][0] = MapElementType::WALL;
        data[i][width - 1] = MapElementType::WALL;
    }
}

// 获取地图宽度
int Map::getWidth() const {
    return width;
}

// 获取地图高度
int Map::getHeight() const {
    return height;
}

// 设置地图上某个位置的元素类型
void Map::setElement(int x, int y, MapElementType type) {
    if (isPositionValid(x, y)) {
        data[y][x] = type;
    }
}

// 获取地图上某个位置的元素类型
MapElementType Map::getElement(int x, int y) const {
    if (isPositionValid(x, y)) {
        return data[y][x];
    }
    return MapElementType::WALL; // 默认返回墙，防止越界
}

// 检查位置是否在地图范围内
bool Map::isPositionValid(int x, int y) const {
    return (x >= 0 && x < width && y >= 0 && y < height);
}

// 清空地图（将所有元素设置为EMPTY）
void Map::clear() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            data[y][x] = MapElementType::EMPTY;
        }
    }
} 