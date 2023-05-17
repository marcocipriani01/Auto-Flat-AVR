# Auto Flat AVR

Port of the Auto Flat project in pure C with AVR libraries.

## Description

Auto Flat is PC-controlled lightbox and dust cover implmented using the Alnitak (Flip-Flat/Flat-Man) command set available [here](https://www.optecinc.com/astronomy/catalog/alnitak/resources/Alnitak_GenericCommandsR4.pdf). It is meant to be used with the Auto Flat Windows ASCOM drivers (closed source, available [on my website](https://marcocipriani01.github.io/projects/Auto_Flat)) or with the included Linux command-line software.

## Serial protocol

```
Send     : ">SOOO\r"      //request state
Recieve  : "*S19OOO\n"    //returned state
Send     : ">B128\r"      //set brightness 128
Recieve  : "*B19128\n"    //confirming brightness set to 128
Send     : ">JOOO\r"      //get brightness
Recieve  : "*B19128\n"    //brightness value of 128 (assuming as set from above)
Send     : ">LOOO\r"      //turn light on (uses set brightness value)
Recieve  : "*L19OOO\n"    //confirms light turned on
Send     : ">DOOO\r"      //turn light off (brightness value does not change, just the state of the light)
Recieve  : "*D19OOO\n"    //confirms light turned off
```
