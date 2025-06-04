#include "bmp_display.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int bmp_display(const char *fbp, struct fb_var_screeninfo *scrinfo, const char *bmp_path, int x, int y)
{
    if (!fbp || !scrinfo || !bmp_path) return -1;

    FILE *fp = fopen(bmp_path, "rb");
    if (!fp) return -1;

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fread(&fileHeader, sizeof(fileHeader), 1, fp);
    fread(&infoHeader, sizeof(infoHeader), 1, fp);

    if (fileHeader.bfType != 0x4D42) { // 'BM'
        fclose(fp);
        return -1;
    }

    int bmp_width = infoHeader.biWidth;
    int bmp_height = infoHeader.biHeight;
    int bmp_bpp = infoHeader.biBitCount / 8;

    // 只支持24位或32位bmp
    if (bmp_bpp != 3 && bmp_bpp != 4) {
        fclose(fp);
        return -1;
    }

    // 跳到像素数据
    fseek(fp, fileHeader.bfOffBits, SEEK_SET);

    int screen_width = scrinfo->xres_virtual;
    int screen_height = scrinfo->yres_virtual;
    int screen_bpp = scrinfo->bits_per_pixel / 8;

    // BMP每行像素字节数为4字节对齐
    int bmp_line_bytes = ((bmp_width * bmp_bpp + 3) / 4) * 4;

    unsigned char *linebuf = (unsigned char*)malloc(bmp_line_bytes);
    if (!linebuf) {
        fclose(fp);
        return -1;
    }

    
    for (int row = 0; row < bmp_height; ++row) {
        // BMP文件从下到上存储
        int screen_y = y + (bmp_height - 1 - row);
        if (screen_y < 0 || screen_y >= screen_height) continue;

        fread(linebuf, 1, bmp_line_bytes, fp);
        for (int col = 0; col < bmp_width; ++col) {
            int screen_x = x + col;
            if (screen_x < 0 || screen_x >= screen_width) continue;

            unsigned char *pixel = linebuf + col * bmp_bpp;
            unsigned int color = 0;
            // 24位BMP
            if (bmp_bpp == 3) {
                color = (0xFF << 24) | (pixel[2] << 16) | (pixel[1] << 8) | pixel[0];
            } else if (bmp_bpp == 4) {
                color = (pixel[3] << 24) | (pixel[2] << 16) | (pixel[1] << 8) | pixel[0];
            }
            unsigned long location = screen_x * screen_bpp + screen_y * screen_width * screen_bpp;
            memcpy((void*)(fbp + location), &color, screen_bpp);
        }
    }

    free(linebuf);
    fclose(fp);
    return 0;
}