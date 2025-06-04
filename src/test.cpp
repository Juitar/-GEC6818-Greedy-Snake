#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstring>
#include "bmp_display.hpp"

#define LCD_W   800
#define LCD_H   480
#define LCD_DEPTH 32
#define BUF_SIZE    (LCD_W*LCD_H*LCD_DEPTH/8)

int main() {
    // 打开 framebuffer 设备
    int fb_fd = open("/dev/fb0", O_RDWR);
    if (fb_fd < 0) {
        perror("open framebuffer failed");
        return -1;
    }

    // 获取屏幕信息
    struct fb_var_screeninfo scrinfo;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &scrinfo) < 0) {
        perror("ioctl get screen info failed");
        close(fb_fd);
        return -1;
    }

    // 映射 framebuffer 内存
    char* fbp = (char*)mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fbp == MAP_FAILED) {
        perror("mmap framebuffer failed");
        close(fb_fd);
        return -1;
    }

    // 清屏（黑色）
    memset(fbp, 0, BUF_SIZE);

    // 显示 BMP 图片
    if (bmp_display(fbp, &scrinfo, "head_up.bmp", 240, 400) != 0) {
        printf("显示BMP图片失败！\n");
    } else {
        printf("BMP图片显示成功！\n");
    }

    // 停留5秒
    sleep(5);

    // 释放资源
    munmap(fbp, BUF_SIZE);
    close(fb_fd);

    return 0;
}