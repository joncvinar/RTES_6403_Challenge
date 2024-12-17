#include <stdint.h>
#include <stdbool.h>

#define DEBOUNCE_THRESHHOLD 200 // Maximum delay detected as a debounce

volatile uint16_t overflowCount = 0; // Counts Timer0 overflows
volatile uint32_t timeElapsed = 0;   // Measured time in ms
volatile bool buttonPressed = false;
volatile uint32_t totalTicks = 0;
volatile uint32_t time = 0;

// Timer0 overflow handler
void Timer0_OV_Handler()
{
    overflowCount++;
}

// Rising edge handler (button released)
void pinRising_Handler()
{
    // Calculate total ticks
    totalTicks = overflowCount * 250 + TCNT0; // TOP value = 250
    time = (totalTicks * 32) / 16000;         // Convert ticks to ms

    if (time > DEBOUNCE_THRESHHOLD)
    {
        timeElapsed = time;
    }
    overflowCount = 0; // Reset overflow count
    TCNT0 = 0;         // Reset Timer0 counter
}

// Falling edge handler (button pressed)
void pinFalling_Handler()
{
    overflowCount = 0; // Reset overflow count
    TCNT0 = 0;         // Reset Timer0 counter
}
