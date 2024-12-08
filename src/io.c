#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

// Peripheral base address for Raspberry Pi 5
#define PERIPHERAL_BASE 0xFE000000
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

// GPIO Register offsets
#define GPFSEL0 (GPIO_BASE + 0x00)  // GPIO Function Select Register 0
#define GPSET0  (GPIO_BASE + 0x1C)  // GPIO Pin Output Set Register 0
#define GPCLR0  (GPIO_BASE + 0x28)  // GPIO Pin Output Clear Register 0
#define GPLEV0  (GPIO_BASE + 0x34)  // GPIO Pin Level Register 0

// Page size for mmap
#define PAGE_SIZE 4096

volatile uint32_t *gpio;

// Function to initialize MMIO
int mmio_init() {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Failed to open /dev/mem");
        return -1;
    }

    // Map the GPIO memory
    gpio = (volatile uint32_t *)mmap(
        NULL,
        PAGE_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        GPIO_BASE
    );

    if (gpio == MAP_FAILED) {
        perror("Failed to mmap GPIO");
        close(mem_fd);
        return -1;
    }

    close(mem_fd);
    printf("MMIO initialized successfully!\n");
    
    if (gpio == NULL) {
    fprintf(stderr, "GPIO pointer is NULL!\n");
    return;
}
    return 0;
}

// Function to clean up MMIO
void mmio_cleanup() {
    if (gpio) {
        munmap((void *)gpio, PAGE_SIZE);
    }
}



// MMIO write function
void mmio_write(uint32_t offset, uint32_t value) {
    if (gpio == NULL) {
        fprintf(stderr, "Error: GPIO pointer is NULL in mmio_write\n");
        return;
    }
    printf("Writing value 0x%08X to offset 0x%08X\n", value, offset);
    gpio[offset / 4] = value;
}

// MMIO read function
uint32_t mmio_read(uint32_t offset) {
    if (gpio == NULL) {
        fprintf(stderr, "Error: GPIO pointer is NULL in mmio_read\n");
        return 0;
    }
    uint32_t value = gpio[offset / 4];
    printf("Read value 0x%08X from offset 0x%08X\n", value, offset);
    return value;
}

void gpio_toggle(int pin) {
    printf("Toggling GPIO pin %d\n", pin);

    if (gpio == NULL) {
        fprintf(stderr, "Error: GPIO pointer is NULL in gpio_toggle\n");
        return;
    }

    // Calculate the register and bit offset
    uint32_t reg_offset = pin / 10;  // Each GPFSEL register configures 10 pins
    uint32_t bit_offset = (pin % 10) * 3;

    printf("Register offset: %u, Bit offset: %u\n", reg_offset, bit_offset);

    // Read GPFSEL (Function Select Register)
    uint32_t gpfsel = mmio_read((reg_offset * 4));
    printf("Current GPFSEL value: 0x%08X\n", gpfsel);

    // Configure the pin as output
    gpfsel &= ~(7 << bit_offset);  // Clear the current pin's bits
    gpfsel |= (1 << bit_offset);   // Set the pin as output
    mmio_write((reg_offset * 4), gpfsel);

    printf("GPIO pin %d set as output.\n", pin);

    // Toggle the GPIO pin
    mmio_write(0x1C, (1 << pin)); // GPSET0: Set the pin
    printf("GPIO pin %d set high.\n", pin);

    sleep(1); // Wait 1 second

    mmio_write(0x28, (1 << pin)); // GPCLR0: Clear the pin
    printf("GPIO pin %d set low.\n", pin);
}

int main() {
    if (mmio_init() != 0) {
        return 1;
    }

    printf("MMIO initialized.\n");

    // Example: Toggle GPIO pin 17
    gpio_toggle(17);

    mmio_cleanup();
    return 0;
}
