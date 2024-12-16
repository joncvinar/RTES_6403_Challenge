#include "mbed.h"

#define IDLE_LOCKED_UNLOCKED 0
#define RECORD 1
#define LOCKING_UNLOCKING 2

// Declare an InterruptIn object for the button
InterruptIn button(BUTTON1);
// Declare a DigitalOut object for the LED
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Timer for debouncing
Timer debounce_timer;

// 0 - Idle/Locked/Unlocked
// 1 - Record
// 2 - Locking/Unlocking
uint8_t state = IDLE_LOCKED_UNLOCKED;

bool is_unlock = true;

// Change the at the falling edge
void button_pressed_fall()
{
  uint32_t time = debounce_timer.read_ms();
  if (time > 100) // Check if 100ms have passed since the last interrupt
  {
    if (time < 2000) // Short press
    {
      state = LOCKING_UNLOCKING;
    }
    else // Long press
    {
      state = is_unlock ? RECORD : IDLE_LOCKED_UNLOCKED;
    }
    debounce_timer.reset(); // Reset the debounce timer
  }
}

// Reset the debounce time at rising edge
void button_pressed_rise()
{
  uint32_t time = debounce_timer.read_ms();
  if (time > 100) // Check if 100ms have passed since the last interrupt
  {
    debounce_timer.reset(); // Reset the debounce timer
  }
}

int main()
{
  debounce_timer.start();

  // Attach the button rise and fall functions
  button.rise(&button_pressed_rise);
  button.fall(&button_pressed_fall);

  while (true)
  {
    if (state == IDLE_LOCKED_UNLOCKED)
    {
      if (is_unlock)
      {
        // Unlocked
        led1.write(1);
        led2.write(0);
      }
      else
      {
        // Locked
        led1.write(0);
        led2.write(1);
      }
    }
    if (state == RECORD)
    {
      // Record
      led1.write(1);
      led2.write(1);

      // Detect gesture
      // Store gesture
      // Change state to IDLE_LOCKED_UNLOCKED
    }
    if (state == LOCKING_UNLOCKING)
    {
      // Attempting to lock/unlock
      led1.write(0);
      led2.write(0);

      // Compare with stored gesture
      // Change is_unlock = true if match, else false
      // Change state to IDLE_LOCKED_UNLOCKED
    }
  }
}
