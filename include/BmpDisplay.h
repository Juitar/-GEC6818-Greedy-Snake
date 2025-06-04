#ifndef BMP_DISPLAY_H
#define BMP_DISPLAY_H

#include <linux/fb.h>
#include <string>

// BMP文件头结构体
#pragma pack(1)
typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;
#pragma pack()

// 在指定位置绘制点
void lcd_draw_point(const char *fbp, struct fb_var_screeninfo *scrinfo, int x, int y, unsigned int color);

// 在指定位置绘制BMP图片（推荐使用）
void lcd_draw_bmp(const char *fbp, struct fb_var_screeninfo *scrinfo, int x0, int y0, const char *path_name);

// 绘制BMP图片并移除指定颜色（实现透明背景效果）
void lcd_draw_bmp_transparent(const char *fbp, struct fb_var_screeninfo *scrinfo, int x0, int y0, const char *path_name, unsigned int transparent_color);

// BMP显示函数（已弃用，保留接口仅为兼容性，内部实现改为调用lcd_draw_bmp）
int bmp_display(const char *fbp, struct fb_var_screeninfo *scrinfo, const char *bmp_path, int x, int y);

#endif // BMP_DISPLAY_H