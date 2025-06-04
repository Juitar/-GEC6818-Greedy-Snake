#ifndef MAP_H
#define MAP_H

#include <vector>

// 地图元素类型
enum class MapElementType {
    EMPTY,
    WALL,
    SNAKE_HEAD,
    SNAKE_BODY,
    FOOD
};

// 地图类
class Map {
private:
    // 地图宽度
    int width;
    // 地图高度
    int height;
    // 地图数据，二维数组表示地图上每个位置的元素类型
    std::vector<std::vector<MapElementType>> data;

public:
    // 构造函数
    Map(int width, int height);
    
    // 初始化地图
    void initialize();
    
    // 获取地图宽度
    int getWidth() const;
    
    // 获取地图高度
    int getHeight() const;
    
    // 设置地图上某个位置的元素类型
    void setElement(int x, int y, MapElementType type);
    
    // 获取地图上某个位置的元素类型
    MapElementType getElement(int x, int y) const;
    
    // 检查位置是否在地图范围内
    bool isPositionValid(int x, int y) const;
    
    // 清空地图（将所有元素设置为EMPTY）
    void clear();
};

#endif // MAP_H 