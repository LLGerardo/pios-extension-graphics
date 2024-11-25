#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
//raspberry pi 5 peri base address
#define MAILBOX_BASE 0xFE000000
#define PAGE_SIZE 4096
#define PAGE_MASK (PAGE_SIZE - 1)

volatile uint32_t *mmap_register(off_t offset) {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Failed to open /dev/mem");
        return NULL;
    }

    // Align the offset to the page boundary
    off_t aligned_offset = offset & ~PAGE_MASK;
    off_t page_offset = offset & PAGE_MASK;

    volatile uint32_t *reg = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, aligned_offset);
    close(mem_fd);

    if (reg == MAP_FAILED) {
        perror("Failed to mmap register");
        return NULL;
    }

    // Return pointer adjusted to the correct offset within the page
    return reg + (page_offset / sizeof(uint32_t));
}

int main() {
    volatile uint32_t *mailbox = mmap_register(MAILBOX_BASE);
    if (!mailbox) {
        return 1;
    }

    printf("Mailbox registers:\n");
    for (int i = 0; i < 16; i++) {
        printf("Offset 0x%X: 0x%08X\n", i * 4, mailbox[i]);
    }

    return 0;
}
FAILED) {
        perror("Failed to map register");
        return NULL;
    }

    // Return pointer adjusted to the correct offset within the page
    return reg + (page_offset / sizeof(uint32_t));
}

int main() {
    volatile uint32_t *mailbox = mmap_register(MAILBOX_BASE);
    if (!mailbox) {
        return 1;
    }

    printf("Mailbox registers:\n");
    for (int i = 0; i < 16; i++) {
        printf("Offset 0x%X: 0x%08X\n", i * 4, mailbox[i]);
    }

    return 0;
}
