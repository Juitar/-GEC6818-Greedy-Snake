#ifndef BMP_DISPLAY_H
#define BMP_DISPLAY_H

#include <linux/fb.h>

// 在指定位置显示bmp图片
// 参数：fbp 显存映射指针，scrinfo 屏幕信息，bmp_path bmp图片路径，x y 显示左上角坐标
int bmp_display(const char *fbp, struct fb_var_screeninfo *scrinfo, const char *bmp_path, int x, int y);

#endif // BMP_DISPLAY_H