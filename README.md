# Auto Flat AVR

Port of the Auto Flat project in pure C with AVR libraries.

## Description

Auto Flat is PC-controlled lightbox and dust cover implmented using the Alnitak (Flip-Flat/Flat-Man) command set available [here](https://www.optecinc.com/astronomy/catalog/alnitak/resources/Alnitak_GenericCommandsR4.pdf). It is meant to be used with the Auto Flat Windows ASCOM drivers (closed source, available [on my website](https://marcocipriani01.github.io/projects/Auto_Flat)) or with the included Linux command-line software.

## Serial protocol

```
Send   :  ">SOOO\r"      // Request state
Receive:  "*S19OOO\n"    // Return state

Send   :  ">B128\r"      // Set brightness to 128
Receive:  "*B19128\n"    // Confirming brightness set to 128

Send   :  ">JOOO\r"      // Get brightness
Receive:  "*B19128\n"    // Brightness value of 128 (assuming it was set from above)

Send   :  ">LOOO\r"      // Turn light on (uses set brightness value)
Receive:  "*L19OOO\n"    // Confirms light turned on

Send   :  ">DOOO\r"      // Turn light off (brightness value does not change, just the state of the light)
Receive:  "*D19OOO\n"    // Confirms light turned off
```

TODO: add the rest of the commands.
