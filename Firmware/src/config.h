#ifndef CONFIG_H
#define CONFIG_H

#define BAUDRATE 9600
#define UART_BUFFERS_SIZE 64

#define FIRMWARE_VERSION 1

////////////////////////////
// EL PANEL CONFIGURATION //
////////////////////////////
#define EL_PANEL_FADE_DELAY 8               // The smaller the value, the faster the panel fades
#define EL_PANEL_LOG_SCALE true             // Log brightness scale (true/false)

////////////////////////////
//  SERVO CONFIGURATION   //
////////////////////////////
#define SERVO_ENABLE true                   // Enable servo motor
#if SERVO_ENABLE == true
#define SERVO_CLOSED_MIN 2400               // Servo pulse width for +15° angle
#define SERVO_CLOSED_MAX 2550               // Servo pulse width for -15° angle
#define SERVO_CLOSED_DEFAULT 2510           // Default closed servo pulse width

#define SERVO_OPEN_MIN 400                  // Servo pulse width for +290° angle
#define SERVO_OPEN_MAX 1300                 // Servo pulse width for +170° angle
#define SERVO_OPEN_DEFAULT 550              // Default value

#define SERVO_STEP_MIN 5                   // Minimum size of one servo step, in μs
#define SERVO_STEP_MAX 50                  // Maximum size of one servo step, in μs
#define SERVO_STEP_DEFAULT 20              // Default size of one servo step, in μs
#endif

////////////////////////////
// ENCODER CONFIGURATION  //
////////////////////////////
#define ENCODER_ENABLE true
#define ENCODER_BRIGHTNESS_STEP 5

#endif
