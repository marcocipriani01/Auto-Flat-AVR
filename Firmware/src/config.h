#ifndef CONFIG_H
#define CONFIG_H

#define BAUDRATE 9600

#define FIRMWARE_VERSION 1

////////////////////////////
// EL PANEL CONFIGURATION //
////////////////////////////
#define EL_PANEL_FADE_DELAY 8               // The smaller the value the faster the panel fades
#define EL_PANEL_LOG_SCALE true             // Log brightness scale (true/false)
#define EL_PANEL_ON_BOOT false              // Panel ON at boot

////////////////////////////
//  SERVO CONFIGURATION   //
////////////////////////////
#define SERVO_ENABLE true                   // Enable servo motor
#if SERVO_ENABLE == true
#define SERVO_STEP_SIZE 10                  // Size of one servo step

#define SERVO_CLOSED_15deg 2400             // Raw servo value for +15°
#define SERVO_CLOSED_m15deg 2550            // Raw servo value for -15°
#define SERVO_CLOSED_DEFAULT 2510           // Default value

#define SERVO_OPEN_290deg 400               // Raw servo value for +290°
#define SERVO_OPEN_170deg 1300              // Raw servo value for +170°
#define SERVO_OPEN_DEFAULT 550              // Default value

#define SERVO_DELAY_MIN 1                   // Delay for the maximum speed
#define SERVO_DELAY_MAX 16                  // Delay for the minimum speed
#define SERVO_DELAY_DEFAULT 7               // Default delay
#endif

////////////////////////////
// ENCODER CONFIGURATION  //
////////////////////////////
#define ENCODER_ENABLE false
#if ENCODER_ENABLE == true
#define ENCODER_CLK_PIN 9
#define ENCODER_DT_PIN 10
#define ENCODER_SW_PIN 8
#define ENCODER_BRIGHTNESS_STEP 5
#endif

#endif
