# PWM Motor Control Project
## Overview
This project controls a small brushed DC motor using an STM32F303VCT6 microcontroller, equipped on an STM32F3 Discovery board, and an L298N motor driver. A potentiometer controls the PWM duty cycle and a 1602A LCD display shows an estimated motor RPM and rotation speed.

## Hardware
* STM32F3 Discovery board
* Small brushed DC motor
* L298N motor driver
* 10kΩ potentiometer
* 16x2 1602A LCD display with PCF8574 I2C adapter
* 1 Green LED, 1 Yellow LED, 1 Red LED 

## Connections
* **PA0**: potentiometer ADC input
* **PA6**: PWM output to the L298N ENA input
* **PB8**: I2C SCL pin for the LCD
* **PB9**: I2C SDA pin for the LCD
* **PD9**: GPIO Output pin for the Green LED
* **PD11**: GPIO Output pin for the Yellow LED
* **PD13**: GPIO Output pin for Red LED
* **Motor direction**: fixed through L298N direction inputs
* All modules must share a **common ground**

## Main features
* Reads potentiometer through ADC1
* Averages 64 ADC sample to reduce noise
* Converts the ADC result to a PWM duty cycle value
* Controls motor power by feeding the PWM signal to the ENA input of the L298N driver 
* Estimates RPM and speed from PWM duty cycle
* Displays RPM and speed on the LCD
* Indicates RPM ranges through the LEDs

## PWM control
The ADC value ranges from 0 to 4095 and is converted to the PWM's duty cycle (ranging from 0 to 999)
```C
duty_cycle = (uint32_t)(((uint64_t)conv * PWM_MAX_COMPARE
           + ADC_MAX_COUNTS / 2U)
           / ADC_MAX_COUNTS);
```

## RPM Estimation
The motor is assumed to start moving at ADC value ~800, roughly 19.5% duty cycle.
```C
#define MOTOR_START_DUTY_CYCLE 195U
#define START_RPM 1200U
#define FULL_DUTY_RPM 12000U
```
Below the estimated starting duty cycle, the RPM is considered to be zero. Between the starting duty and full duty, the RPM scales linearly, from 1,200 to 12,000 RPM.

## Speed Estimation
Propeller speed is computed using the following formula:
```
Propeller speed (km/h) = RPM × π × propeller diameter (m) × 60 / 1000
```
The configured propeller diameter is 60 mm and the maximum speed is considered 136km/h @ 12,000 RPM.



## LCD output
The LCD displays:
```
RPM: 12000
Speed: 136 km/h
```
Strings have a fixed-width and trailing spaces are used to ensure digits from previous reads have been erased.

## LED output
* ADC below 800: all LEDs are turned off
* ADC from 800 to 1999: Green LED on
* ADC from 2000 to 3499: Green and Yellow LEDs are turned on
* ADC above 3500: Green, Yellow and Red LEDs are turned on
