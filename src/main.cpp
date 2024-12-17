#Create parameters for registering x,y,z movements (10?)

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

const float x_threshold = 2.0; // Change this value as needed
const float y_threshold = 2.0;
const float z_threshold = 2.0;

// 0 - Idle/Locked/Unlocked
// 1 - Record
// 2 - Locking/Unlocking
uint8_t state = IDLE_LOCKED_UNLOCKED;

bool is_unlock = true;

// Placeholder function for getting gyroscope data (simulated)
void get_gyro_data(float *x, float *y, float *z) {
    *x = rand() % 10 - 5; // Simulate x-axis gyro data
    *y = rand() % 10 - 5; // Simulate y-axis gyro data
    *z = rand() % 10 - 5; // Simulate z-axis gyro data
}

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
      // Get gyroscope data
      get_gyro_data(&x, &y, &z);

      // Check if movement exceeds threshold
      if (abs(x) > x_threshold || abs(y) > y_threshold || abs(z) > z_threshold) {
          printf("Movement detected! X: %.2f, Y: %.2f, Z: %.2f\n", x, y, z);
          // TODO: Store movement data

      

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
