#include "../include/BmpDisplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>

// 在指定位置绘制点
void lcd_draw_point(const char *fbp, struct fb_var_screeninfo *scrinfo, int x, int y, unsigned int color) {
    if (!fbp || !scrinfo) return;
    
    int screen_width = scrinfo->xres_virtual;
    int screen_height = scrinfo->yres_virtual;
    int screen_bpp = scrinfo->bits_per_pixel / 8;
    
    // 检查坐标是否在屏幕范围内
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return;
    }
    
    // 计算像素在帧缓冲中的位置
    unsigned long location = (x + y * screen_width) * screen_bpp;
    
    // 写入颜色值
    memcpy((void*)(fbp + location), &color, screen_bpp);
}

// 在指定位置绘制BMP图片
void lcd_draw_bmp(const char *fbp, struct fb_var_screeninfo *scrinfo, int x0, int y0, const char *path_name) {
    if (!fbp || !scrinfo || !path_name) return;
    
    // 打开图片文件
    int fd_pic = open(path_name, O_RDONLY);
    if (fd_pic == -1) {
        perror("open pic error");
        return;
    }
    
    // 读取图片文件的信息(魔数、像素数组偏移量、宽度、高度、色深)
    short MS, depth; // 魔数、色深
    int offset, w, h; // 像素数组的文件偏移量、宽度、高度
    
    // 读取魔数
    read(fd_pic, &MS, 2);
    if (MS != 0x4D42) {
        // 不是BMP图片
        printf("This pic is not BMP!\n");
        close(fd_pic);
        return;
    }
    
    // 读取偏移量、宽度、高度和色深
    lseek(fd_pic, 0x0A, SEEK_SET);
    read(fd_pic, &offset, 4);
    
    lseek(fd_pic, 0x12, SEEK_SET);
    read(fd_pic, &w, 4);
    read(fd_pic, &h, 4);
    
    lseek(fd_pic, 0x1C, SEEK_SET);
    read(fd_pic, &depth, 2);
    
    // 读取像素数组数据
    int full_bytes = (4 - (w * depth / 8) % 4) % 4; // (4-多出字节数)%4
    int color_buf_size = (w * depth / 8 + full_bytes) * abs(h); // 所有像素点颜色值大小+所有填充字节数
    
    // 分配内存
    unsigned char *color_buf = (unsigned char*)malloc(color_buf_size);
    if (!color_buf) {
        close(fd_pic);
        return;
    }
    
    // 读取像素数据
    lseek(fd_pic, offset, SEEK_SET);
    ssize_t bytes_read = read(fd_pic, color_buf, color_buf_size);
    if (bytes_read != color_buf_size) {
        printf("Failed to read complete BMP data\n");
        free(color_buf);
        close(fd_pic);
        return;
    }
    
    // 遍历整个图像上的像素点
    unsigned char a, r, g, b; // 4个颜色分量
    unsigned char *p = color_buf; // 为了方便访问数据，通过指针p来访问
    
    for (int y = 0; y < abs(h); y++) {
        for (int x = 0; x < w; x++) {
            // 将该像素点的颜色值按照ARGB的顺序重新排列组合，显示到屏幕上去
            b = *p++;
            g = *p++;
            r = *p++;
            
            // 判断是24位还是32位
            if (depth == 24) {
                // 只有颜色分量BGR，没有A，需要手动赋值
                a = 0xFF; // 完全不透明
            } else if (depth == 32) {
                // 有颜色分量BGRA，可以直接获取A的颜色分量值
                a = *p++;
            } else {
                // 不支持的色深
                continue;
            }
            
            // 按照ARGB的顺序进行重新排列
            unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;
            
            // 显示颜色值
            if (h > 0) {
                // 高度值为正数，图片在保存时的扫描顺序为：从下到上
                // 所以，图片的显示顺序也应该从下到上显示
                lcd_draw_point(fbp, scrinfo, x + x0, h - 1 - y + y0, color);
            } else {
                // 高度值为负数，按照从上到下的顺序显示
                lcd_draw_point(fbp, scrinfo, x + x0, y + y0, color);
            }
        }
        
        // 每显示完一行像素点的颜色值，就跳过后面的填充字节
        p += full_bytes;
    }
    
    // 释放内存并关闭文件
    free(color_buf);
    close(fd_pic);
}

// 绘制BMP图片并移除指定颜色（实现透明背景效果）
void lcd_draw_bmp_transparent(const char *fbp, struct fb_var_screeninfo *scrinfo, int x0, int y0, const char *path_name, unsigned int transparent_color) {
    if (!fbp || !scrinfo || !path_name) return;
    
    // 打开图片文件
    int fd_pic = open(path_name, O_RDONLY);
    if (fd_pic == -1) {
        perror("open pic error");
        return;
    }
    
    // 读取图片文件的信息(魔数、像素数组偏移量、宽度、高度、色深)
    short MS, depth; // 魔数、色深
    int offset, w, h; // 像素数组的文件偏移量、宽度、高度
    
    // 读取魔数
    read(fd_pic, &MS, 2);
    if (MS != 0x4D42) {
        // 不是BMP图片
        printf("This pic is not BMP!\n");
        close(fd_pic);
        return;
    }
    
    // 读取偏移量、宽度、高度和色深
    lseek(fd_pic, 0x0A, SEEK_SET);
    read(fd_pic, &offset, 4);
    
    lseek(fd_pic, 0x12, SEEK_SET);
    read(fd_pic, &w, 4);
    read(fd_pic, &h, 4);
    
    lseek(fd_pic, 0x1C, SEEK_SET);
    read(fd_pic, &depth, 2);
    
    // 读取像素数组数据
    int full_bytes = (4 - (w * depth / 8) % 4) % 4; // (4-多出字节数)%4
    int color_buf_size = (w * depth / 8 + full_bytes) * abs(h); // 所有像素点颜色值大小+所有填充字节数
    
    // 分配内存
    unsigned char *color_buf = (unsigned char*)malloc(color_buf_size);
    if (!color_buf) {
        close(fd_pic);
        return;
    }
    
    // 读取像素数据
    lseek(fd_pic, offset, SEEK_SET);
    ssize_t bytes_read = read(fd_pic, color_buf, color_buf_size);
    if (bytes_read != color_buf_size) {
        printf("Failed to read complete BMP data\n");
        free(color_buf);
        close(fd_pic);
        return;
    }
    
    // 遍历整个图像上的像素点
    unsigned char a, r, g, b; // 4个颜色分量
    unsigned char *p = color_buf; // 为了方便访问数据，通过指针p来访问
    
    for (int y = 0; y < abs(h); y++) {
        for (int x = 0; x < w; x++) {
            // 将该像素点的颜色值按照ARGB的顺序重新排列组合，显示到屏幕上去
            b = *p++;
            g = *p++;
            r = *p++;
            
            // 判断是24位还是32位
            if (depth == 24) {
                // 只有颜色分量BGR，没有A，需要手动赋值
                a = 0xFF; // 完全不透明
            } else if (depth == 32) {
                // 有颜色分量BGRA，可以直接获取A的颜色分量值
                a = *p++;
            } else {
                // 不支持的色深
                continue;
            }
            
            // 按照ARGB的顺序进行重新排列
            unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;
            
            // 如果颜色等于透明色，则跳过该像素
            if (color == transparent_color) {
                continue;
            }
            
            // 显示颜色值
            if (h > 0) {
                // 高度值为正数，图片在保存时的扫描顺序为：从下到上
                // 所以，图片的显示顺序也应该从下到上显示
                lcd_draw_point(fbp, scrinfo, x + x0, h - 1 - y + y0, color);
            } else {
                // 高度值为负数，按照从上到下的顺序显示
                lcd_draw_point(fbp, scrinfo, x + x0, y + y0, color);
            }
        }
        
        // 每显示完一行像素点的颜色值，就跳过后面的填充字节
        p += full_bytes;
    }
    
    // 释放内存并关闭文件
    free(color_buf);
    close(fd_pic);
}

// 为保持兼容性，保留bmp_display函数接口，但内部实现改为调用lcd_draw_bmp
int bmp_display(const char *fbp, struct fb_var_screeninfo *scrinfo, const char *bmp_path, int x, int y) {
    // 调用lcd_draw_bmp实现功能
    lcd_draw_bmp(fbp, scrinfo, x, y, bmp_path);
    return 0;
} 