# Temperature Control System (Microprocessors Course Project)
This Project is to design temperature control system using two ATMega32 microcontrollers.

- Uses SPI communication protocol
- Uses LM35 Temperature Sensor for measurement of temperature
- 16×2 LCD is used to display temperature set point
- It controls temperature by turning on and off the heater or cooler
- Show warning if the temperature is in the bad state

# Master Chip
- Receives two temperature values A and B from LM35 sensors
- Compares the two values using AC
- As long as A is higher than B, converts the analog value of A to digital using ADC
- Prints the digital value of A on a 16x2 alphanumeric LCD
- Sends the digital value of A to the slave

# Slave Chip
- Receives the temperature value from the master
- Turn on the cooler motor for temperatures between 25 and 55 degrees (starting with a
duty cycle of 50% plus 10% for every additional 5 degrees)
- Turn on the heater when the temperature is lower than 20 degrees
- Red warning LED blink when the temperature is higher than 55 degrees
- When one motor is on, the other motor should be off

# Design
![DesignImage](https://github.com/Rez79Kh/Temperature-Control-System/blob/main/Design/DesignImage.png?raw=true)
