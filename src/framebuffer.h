#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    int pitch;
    uint32_t *buffer;
    int fd;
} Framebuffer;

int fb_init(Framebuffer *fb);
void fb_clear(Framebuffer *fb, uint32_t color);
void fb_put_pixel(Framebuffer *fb, int x, int y, uint32_t color);
void fb_draw_line(Framebuffer *fb, int x0, int y0, int x1, int y1, uint32_t color);
void fb_draw_rect(Framebuffer *fb, int x, int y, int width, int height, uint32_t color);
void fb_cleanup(Framebuffer *fb);

#endif