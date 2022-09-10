# Arduino Powered 4-Speed Fan with Auto Shutoff

As the name implies, the Arduino 4-Speed Fan is just that - a fan that has 4 speeds. The speed can be set using either the keypad or dedicated buttons. As a safety feature, the fan is equipped with an ultrasonic sensor, and will automatically turn off if any object is detected within 10 cm of the sensor.

[**Demo Video**](https://youtu.be/u6rcp0OhNVM) 

## Hardware Components Used
* 1x Elegoo UNO R3 Microcontroller (ATmega328P)
* 1x Breadboard
* 2x Buttons
  * 1x PULLDOWN Configuration (X)
  * 1x PULLUP Configuration (Y)
* 1x 1k Ohm Resistor
* 4x LEDs
* 5x 220 Ohm Resistors
* 1x Diode Rectifier 1N4007
* 1x NPN Transistor PN2222
* 1x Fan Attachment
* 1x DC Motor
* 1x 4x4 Keypad
* 1x Ultrasonic Sensor
* 21x Wires

## Software Libraries Used
* Keypad Library for Arduino (Keypad.h)
  * Used for setting up, defining, and reading the keypad inputs/buttons.
  * Used the makeKeymap() function to set up and define the buttons on the 4x4 keypad. Used the getKey() function to read the keypad inputs/buttons.
  * The software library allowed me to read user inputs from the keypad and adjust the fan speed based on the input.

## Wiring Diagram
<img src="/WiringDiagrams/Diagram1.PNG"/>
<img src="/WiringDiagrams/Diagram2.PNG"/>
