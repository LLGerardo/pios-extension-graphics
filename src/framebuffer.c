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
