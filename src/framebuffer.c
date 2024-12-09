#include "framebuffer.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
        printf("pixel printed");
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

void fb_draw_circle(Framebuffer *fb, int x0, int y0, int radius, uint32_t color) {
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        fb_put_pixel(fb, x0 + x, y0 + y, color);
        fb_put_pixel(fb, x0 + y, y0 + x, color);
        fb_put_pixel(fb, x0 - y, y0 + x, color);
        fb_put_pixel(fb, x0 - x, y0 + y, color);
        fb_put_pixel(fb, x0 - x, y0 - y, color);
        fb_put_pixel(fb, x0 - y, y0 - x, color);
        fb_put_pixel(fb, x0 + y, y0 - x, color);
        fb_put_pixel(fb, x0 + x, y0 - y, color);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void fb_draw_heart(Framebuffer *fb, int x, int y, int size, uint32_t color) {
    int width = size;
    int height = size;
    
    for (int i = -width/2; i <= width/2; i++) {
        for (int j = -height/2; j <= height/2; j++) {
            float dx = (float)i / (width/2);
            float dy = (float)j / (height/2);
            if ((dx*dx + dy*dy - 1)*(dx*dx + dy*dy - 1)*(dx*dx + dy*dy - 1) - dx*dx*dy*dy*dy <= 0.0) {
                fb_put_pixel(fb, x+i, y+j, color);
            }
        }
    }
}

void fb_draw_smiley(Framebuffer *fb, int x, int y, int radius, uint32_t color) {
    // face
    fb_draw_circle(fb, x, y, radius, color);
    
    // eyes
    int eye_radius = radius / 5;
    fb_draw_circle(fb, x - radius/3, y - radius/3, eye_radius, color);
    fb_draw_circle(fb, x + radius/3, y - radius/3, eye_radius, color);
    
    // smile
    int smile_radius = radius * 2 / 3;
    for (int i = -smile_radius; i <= smile_radius; i++) {
        int h = (int)(sqrt(smile_radius*smile_radius - i*i) / 1.5);
        fb_put_pixel(fb, x + i, y + h, color);
    }
}

void fb_cleanup(Framebuffer *fb) {
    munmap(fb->buffer, fb->pitch * fb->height);
    close(fb->fd);
}