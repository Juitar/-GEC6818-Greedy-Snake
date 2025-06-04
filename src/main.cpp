#include <iostream>
#include <thread>
#include <chrono>
#include "../include/Game.h"

int main() {
    std::cout << "Game Starting..." << std::endl;
    
    Game game(20,12);
    // 初始化游戏
    game.initialize();
    
    // 开始游戏（这将启动所有游戏线程）
    game.start();
    
    // 主线程可以处理用户输入或其他任务
    while (game.getState() != GameState::EXIT) {
        // 检查游戏是否结束
        if (game.getState() == GameState::GAME_OVER) {
            std::cout << "Game Over! Your score is " << game.getScore() << std::endl;
            
            // 这里模拟用户输入
            std::cout << "Click anywhere to restart the game..." << std::endl;
            
            // 等待一段时间后自动重启游戏
            std::this_thread::sleep_for(std::chrono::seconds(3));
            
            // 重置游戏
            game.reset();
            game.start();
        }
        
        // 控制主循环频率
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // 等待所有游戏线程结束
    game.waitForThreads();
    
    std::cout << "Game Over." << std::endl;
    return 0;
}
