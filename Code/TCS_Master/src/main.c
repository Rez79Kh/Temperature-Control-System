#include <avr/io.h>
#include <util/delay.h>
#include <Lcd.h>

void print_output(unsigned char);
void print_degree_sign();
void convert_to_digital();
void cmp_temperatures();
void spi_master(unsigned char);

int main(void){
    cmp_temperatures();
}

/**
 * AC (Compare analog values of LM35 sensors)
 */
void cmp_temperatures(){
    DDRD = (1<<DDD0); // set the D0 pin as output of AC
    SFIOR |= (0<<ACME); // disable ADC
    ACSR  |= (0<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

    while(1){
        if(((ACSR >> ACO) & 1) == 1){
            // A > B => Convert Analog value to digital
            convert_to_digital();
        }
        else{
            // A < B => Nothing happen
            LCD_cmd(0x01); // Clear LCD screen
        }
    }
}

/**
 * ADC (Convert Analog value to digital)
 */
void convert_to_digital(){
    // declare PORTD and PORTC as output
    DDRC = 0xFF;
    DDRD = 0xFF;
    // Initialize LCD
    init_LCD();
    // Initialize ADC
    ADMUX = (0 << REFS1) | (1 << REFS0); // Vref come from AVCC (other 5 bit = 0 => pinA0 is input)
    ADCSRA = (1<<ADEN) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);  // Enable , prescaling 8
    LCD_cmd(0x01); // Clear LCD Screen
    LCD_cmd(0x84);
    // Convert
    ADCSRA |= ((1 << ADSC) | (1 << ADIF)); // start
    while( (ADCSRA & (1 << ADIF)) == 0 ); // when interrupt flag = 1 => conversion end
    print_output(ADC/2); // Print the value
    _delay_ms(200);
    spi_master(ADC/2); // Immediately send data
}

/**
 * SPI master , Send data to slave
 * @param data the data that wants to send to slave
 */
void spi_master(unsigned char data){
    DDRB = (1<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3);
    // SPI initialization
    SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0); // Enable , Prescalling 128
    SPSR = (0<<SPI2X); // double spi speed

    PORTB &= ~(1<<PORTB4); // Select Slave
    SPDR = data; // data register => Send Data
    while(((SPSR >> SPIF) & 1) == 0); // interrupt flag => data sent
    PORTB |= (1<<PORTB4); // Deselect Slave
}

/**
 * Print the degree sign on LCD
 */
void print_degree_sign(){
  LCD_cmd(0x40);
  LCD_write(10);
  LCD_write(17);
  LCD_write(17);
  LCD_write(10);
  LCD_write(0);
  LCD_write(0);
  LCD_write(0);
  LCD_write(0);
  LCD_cmd(0x87);
  LCD_write(0x00);
  LCD_write('C');
}

/**
 * Print the Temperature digital value on LCD
 * @param num Number that wants to show on LCD
 */
void print_output(unsigned char num){
  unsigned int remaining;
  remaining = num/10;
  LCD_write((remaining/10) + 48);
  LCD_write((remaining%10) + 48);
  LCD_write((num%10) + 48);
  print_degree_sign();
}