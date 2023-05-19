#ifndef CONFIG_H
#define CONFIG_H

#define BAUDRATE 9600

////////////////////////////
// EL PANEL CONFIGURATION //
////////////////////////////
#define EL_PANEL_FADE_DELAY 8                           // The smaller the value the faster the panel fades
#define LOG_SCALE false                                  // Log brightness scale (true/false)
#define EL_PANEL_ON_BOOT false                          // Panel ON at boot

////////////////////////////
//  SERVO CONFIGURATION   //
////////////////////////////
//#define SERVO_PIN 5                                     // Servo pin
#ifdef SERVO_PIN
#define STEP_SIZE 10                                    // Size of one servo step

#define CLOSED_SERVO_15deg 2400                         // Raw servo value for +15°
#define CLOSED_SERVO_m15deg 2550                        // Raw servo value for -15°
#define CLOSED_SERVO_DEFAULT 2510                       // Default value

#define OPEN_SERVO_290deg 400                           // Raw servo value for +290°
#define OPEN_SERVO_170deg 1300                          // Raw servo value for +170°
#define OPEN_SERVO_DEFAULT 550                          // Default value

#define SERVO_DELAY_MIN 1000                            // Delay for the maximum speed
#define SERVO_DELAY_MAX 16300                           // Delay for the minimum speed
#define SERVO_DELAY_DEFAULT 7000                        // Default delay
#endif

////////////////////////////
// ENCODER CONFIGURATION  //
////////////////////////////
#define KNOB_CONTROL false
#if KNOB_CONTROL == true
#define ENCODER_CLK_PIN 9
#define ENCODER_DT_PIN 10
#define ENCODER_SW_PIN 8
#define ENCODER_BRIGHTNESS_STEP 5
#endif

#endif
