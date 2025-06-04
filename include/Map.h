#ifndef MAP_H
#define MAP_H

#include <vector>

// 地图元素类型
enum class MapElementType {
    EMPTY,
    WALL,
    SNAKE_HEAD,
    SNAKE_BODY,
    FOOD,
    FOOD_APPLE,   // 苹果
    FOOD_PEPPER,  // 辣椒
    FOOD_MEAT,    // 肉
    FOOD_BOMB     // 炸弹
};

// 地图类
class Map {
private:
    int width;  // 地图宽度
    int height; // 地图高度
    std::vector<std::vector<MapElementType>> grid; // 二维网格表示地图

public:
    // 构造函数
    Map(int width, int height);
    
    // 获取地图宽度
    int getWidth() const;
    
    // 获取地图高度
    int getHeight() const;
    
    // 获取指定位置的元素类型
    MapElementType getElement(int x, int y) const;
    
    // 设置指定位置的元素类型
    void setElement(int x, int y, MapElementType element);
    
    // 清空地图（将所有元素设为EMPTY）
    void clear();
};

#endif // MAP_H