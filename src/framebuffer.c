#include "framebuffer.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

int fb_init(Framebuffer *fb) {
    fb->fd = open("/dev/fb0", O_RDWR);
    if (fb->fd == -1) {
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        close(fb->fd);
        return -1;
    }

    fb->width = vinfo.xres;
    fb->height = vinfo.yres;
    fb->pitch = vinfo.xres * 4;
    
    fb->buffer = mmap(0, fb->pitch * fb->height, PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);
    if (fb->buffer == MAP_FAILED) {
        close(fb->fd);
        return -1;
    }

    return 0;
}

void fb_clear(Framebuffer *fb, uint32_t color) {
    for (int y = 0; y < fb->height; y++) {
        for (int x = 0; x < fb->width; x++) {
            fb->buffer[y * fb->width + x] = color;
        }
    }
}

void fb_put_pixel(Framebuffer *fb, int x, int y, uint32_t color) {
    if (x >= 0 && x < fb->width && y >= 0 && y < fb->height) {
        fb->buffer[y * fb->width + x] = color;
    }
}

void fb_draw_line(Framebuffer *fb, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        fb_put_pixel(fb, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void fb_draw_rect(Framebuffer *fb, int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            fb_put_pixel(fb, x + i, y + j, color);
        }
    }
}

void fb_cleanup(Framebuffer *fb) {
    munmap(fb->buffer, fb->pitch * fb->height);
    close(fb->fd);
}