#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "framebuffer.h"
//#include "mb.c"

#define COLOR_RED   0xFF0000
#define COLOR_GREEN 0x00FF00
#define COLOR_BLUE  0x0000FF
#define COLOR_WHITE 0xFFFFFF
#define COLOR_YELLOW 0xFFFF00

int main() {
    Framebuffer fb;
    
    if (fb_init(&fb) != 0) {
        fprintf(stderr, "Failed to initialize framebuffer\n");
        return 1;
    }

    // Clear the screen to black
    fb_clear(&fb, 0x000000);

    // Draw some shapes
    fb_draw_line(&fb, 0, 0, fb.width - 1, fb.height - 1, COLOR_RED);
    printf("This is a line printed to the screen.\n");
    sleep(3);
    fb_draw_rect(&fb, 100, 100, 200, 150, COLOR_GREEN);
    printf("This is a rectangle printed to the screen.\n");
    sleep(3);
    fb_draw_heart(&fb, fb.width / 4, fb.height / 4, 100, COLOR_RED);
    printf("This is a heart printed to the screen.\n");
    sleep(3);
    fb_draw_smiley(&fb, fb.width * 3 / 4, fb.height * 3 / 4, 75, COLOR_YELLOW);
    printf("This is a smiley face printed to the screen.\n");

    // Keep the display for 10 seconds
    sleep(10);

    fb_cleanup(&fb);
    return 0;
}