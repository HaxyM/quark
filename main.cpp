#include <stdint.h>

// Replace with your target's hardware-specific includes if needed

extern "C" void _start() {
    // Bare-metal entry point
    volatile uint32_t counter = 0;
    while (1) {
        counter++;
    }
}
