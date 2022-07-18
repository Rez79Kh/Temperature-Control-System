#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

double dutyCycle = 50; // 50% dutyCycle
void slave();
void check(int);
void turn_cooler(int,int);
void turn_heater(int);
void turn_on_led();

int main(void){
    slave();
}

/**
 * Slave Spi , Get the data from Master
 */
void slave(){
  // SPI initialization
  DDRB = (0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4);
  SPCR = (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0); // Enable , Prescalling 128
  SPSR = (0<<SPI2X); // double spi speed

  while(1) {
      while (((SPSR >> SPIF) & 1) == 0); // interrupt flag => data received
      check(SPDR);
  }
}

/**
 * Check The temperature and decide to turn devices on or off
 * @param temperature its the temperature of Lm35 sensor
 */
void check(int temperature){
  // if 25 < Temperature < 55  =>  Cooler On AND Heater Off
  if(temperature > 25 && temperature < 55){
    turn_heater(0); // Turn off the heater (on_off_flag = 0)
    // Turn On the Cooler with dutyCycle = 50% (on_off_flag = 1 , add_degree)
    int add_degree = (temperature-25)/5;
    turn_cooler(1,add_degree);
  }

  // if Temperature < 20  =>  Heater On AND Cooler Off
  else if(temperature < 20){
    turn_cooler(0,0); // Turn off the cooler (on_off_flag = 0 , add_degree = 0)
    turn_heater(1); // Turn On the heater with dutyCycle = 50% (on_off_flag = 1)
  }
  
  // if Temperature > 55  =>  RED Warning LED AND Cooler Off
  else if (temperature > 55){
    turn_cooler(0,0); // Turn off the cooler (on_off_flag = 0 , add_degree = 0)
    turn_heater(0); // Turn off the heater (on_off_flag = 0)
    turn_on_led(); // RED Warning LED Blink
  }

  else{
    turn_heater(0); // Turn off the heater (on_off_flag = 0)
    turn_cooler(0,0); // Turn off the cooler (on_off_flag = 0 , add_degree = 0)
  }
}

/**
 * Turn the Cooler motor on or off
 * @param on_off_flag flag for decide to turn the device on or off (1 => Turn on , 0 => Turn off)
 * @param add_degree it shows how many ten percent should be added to dutyCycle
 */
void turn_cooler(int on_off_flag,int add_degree){
  DDRB |= (1<<PORTB3); // OC0 Pin
  TCCR0 = 0x00;
  TCCR0 |= (1<<WGM01) | (1<<WGM00); // set Fast PWM mode with non-inverted output
  TCCR0 |= (on_off_flag<<COM01); // if flag = 1 => Clear OC0 on compare match , else disconnected
  TCCR0 |= (1<<CS01); // clock Prescaling 8
  OCR0 = (dutyCycle/100)*255; // dutyCycle
  TIMSK |= (1<<TOIE0);
  sei();
      
  if(add_degree>=1){
    _delay_ms(50);
    dutyCycle += (add_degree*10);
  }
  
  if (dutyCycle > 100){
      dutyCycle = 50;
  }
}

/**
 * Set dutyCycle when OVF interrupt happens
 */
ISR (TIMER0_OVF_vect){
    OCR0 = (dutyCycle/100)*255;
}

/**
 * Turn the heater motor on or off
 * @param on_off_flag flag for decide to turn the device on or off (1 => Turn on , 0 => Turn off)
 */
void turn_heater(int on_off_flag){
  DDRD |= (1<<PORTD7); // OC2 Pin
  TCCR2 = 0x00;
  TCCR2 |= (1<<WGM21) |(1<<WGM20); // Fast PWM
  TCCR2 |= (on_off_flag<<COM21); // if flag = 1 => Clear OC2 on compare match , else disconnected
  TCCR2 |= (1<<CS21); // clock Prescaling 8
  OCR2 = (dutyCycle/100)*255; // dutyCycle
}

/**
 * Make a RED Warning LED Blink
 * @param on_off_flag flag for decide to turn the device on or off (1 => Turn on , 0 => Turn off)
 */
void turn_on_led(){
  DDRD |= (1<<PORTD0); // declared PinD0 as output
  PORTD = 0x01; // turn on LED
  _delay_ms(100);
  PORTD = 0x00; // turn off LED
  _delay_ms(100);
}