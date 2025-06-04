#include <iostream>
#include <thread>
#include <chrono>
#include "../include/Game.h"

// 模拟输入函数，用于测试
Direction getInput() {
    // 这里将来会接入真实的触摸输入
    // 现在只是简单返回一个方向
    return Direction::RIGHT;
}

int main() {
    std::cout << "贪吃蛇游戏启动中..." << std::endl;
    
    // 创建游戏对象，地图大小为20x20
    Game game(20, 20);
    
    // 初始化游戏
    game.initialize();
    
    // 开始游戏
    game.start();
    
    // 游戏主循环
    while (game.getState() != GameState::EXIT) {
        // 处理输入
        Direction input = getInput();
        game.handleInput(input);
        
        // 更新游戏状态
        game.update();
        
        // 渲染游戏
        game.render();
        
        // 根据游戏状态处理
        if (game.getState() == GameState::GAME_OVER) {
            std::cout << "游戏结束！得分：" << game.getScore() << std::endl;
            std::cout << "按任意键重新开始..." << std::endl;
            
            // 这里将来会等待用户输入
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            // 重置游戏
            game.reset();
            game.start();
        }
        
        // 控制游戏速度
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "游戏已退出。" << std::endl;
    return 0;
}
