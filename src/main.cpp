#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>  // 添加PATH_MAX的头文件
#include <libgen.h>  // 添加dirname函数的头文件
#include "../include/Game.h"

// 检查目录是否存在
bool directoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR);
}

// 获取可执行文件所在的目录
std::string getExecutableDir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        result[count] = '\0';
        return std::string(dirname(result));
    }
    return ".";
}

// 主函数
int main(int argc, char* argv[]) {
    // 设置屏幕大小和单元格大小
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 480;
    const int CELL_SIZE = 40;
    
    // 获取可执行文件所在的目录
    std::string execDir = getExecutableDir();
    std::cout << "Executable directory: " << execDir << std::endl;
    
    // 检查资源路径参数
    std::string resourcePath = execDir + "/assets/pic";
    if (argc > 1) {
        // 如果提供了命令行参数，使用绝对路径
        if (argv[1][0] == '/') {
            // 已经是绝对路径
            resourcePath = argv[1];
        } else {
            // 相对路径，转换为绝对路径
            char absPath[PATH_MAX];
            if (realpath(argv[1], absPath) != nullptr) {
                resourcePath = absPath;
            } else {
                // 如果转换失败，尝试基于当前目录构建路径
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                    resourcePath = std::string(cwd) + "/" + argv[1];
                }
            }
        }
    }
    
    std::cout << "Starting Greedy Snake Game..." << std::endl;

    // 检查资源路径是否存在
    if (!directoryExists(resourcePath)) {
        std::cerr << "Error: Resource directory does not exist: " << resourcePath << std::endl;
        
        // 尝试其他可能的路径
        std::vector<std::string> possiblePaths = {
            "./assets/pic",
            "../assets/pic",
            "../../assets/pic"
        };
        
        std::cout << "Trying alternative paths..." << std::endl;
        for (const auto& path : possiblePaths) {
            std::cout << "Checking: " << path << std::endl;
            if (directoryExists(path)) {
                std::cout << "Found valid path: " << path << std::endl;
                resourcePath = path;
                break;
            }
        }
        
        if (!directoryExists(resourcePath)) {
            std::cerr << "Could not find a valid resource directory." << std::endl;
            std::cerr << "Please make sure the directory exists and contains the required BMP files." << std::endl;
            std::cerr << "Usage: " << argv[0] << " [resource_path]" << std::endl;
            return 1;
        }
    }
    
    // 检查资源文件是否存在
    std::vector<std::string> requiredFiles = {
        "/head_up.bmp",
        "/head_down.bmp",
        "/head_left.bmp",
        "/head_right.bmp",
        "/body_up&down.bmp",
        "/body_left&right.bmp",
        "/apple.bmp",
        "/grass1.bmp",
        "/grass2.bmp"
    };
    
    std::cout << "Checking required files in: " << resourcePath << std::endl;
    bool allFilesExist = true;
    for (const auto& file : requiredFiles) {
        std::string fullPath = resourcePath + file;
        if (access(fullPath.c_str(), F_OK) == -1) {
            std::cerr << "Error: Required file not found: " << fullPath << std::endl;
            allFilesExist = false;
        } else {
            std::cout << "Found: " << file << std::endl;
        }
    }
    
    if (!allFilesExist) {
        std::cerr << "Some required files are missing. Please check the resource directory." << std::endl;
        return 1;
    }
    
    try {
        // 创建游戏对象
        Game game(SCREEN_WIDTH, SCREEN_HEIGHT, CELL_SIZE, resourcePath);
        
        // 初始化游戏
        if (!game.initialize()) {
            std::cerr << "Failed to initialize game" << std::endl;
            return 1;
        }
        
        // 开始游戏
        game.start();
        
        // 等待游戏结束
        while (game.getState() != GameState::EXIT) {
            // 检查游戏是否结束
            if (game.getState() == GameState::GAME_OVER) {
                std::cout << "Game Over! Exiting..." << std::endl;
                
                // 等待一段时间后退出游戏
                sleep(3);
                
                // 设置游戏状态为EXIT，直接退出游戏
                game.exit();
            }
            
            // 控制主循环频率
            sleep(1);
        }
        
        // 等待所有游戏线程结束
        game.waitForThreads();
        
        std::cout << "Game exited." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
