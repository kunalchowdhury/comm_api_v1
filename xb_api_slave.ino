#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU_1 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU_1 / (USART_BAUDRATE * 16UL))) - 1)
#define UART_SENDER_BUFFER_SZ 0x1F

uint8_t buffer[UART_SENDER_BUFFER_SZ];

volatile uint8_t sz;
volatile uint8_t eeprom_val;

void write_eeprom(uint8_t c, uint8_t add)
{
   while(EECR & (1 << EEPE)); 
   EEAR = add;
   EEDR = c;
   EECR |= (1 << EEMPE);
   EECR |= (1 << EEPE);
  
}


void uart_init(bool send){
  if(send)
  {
    UCSR0B = (1 << TXEN0);
  }else
  {
    UCSR0B = (1 << RXEN0);
  }
  
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UBRR0H = (uint8_t)(UBRR_VALUE >> 8) ;
  UBRR0L = (uint8_t) UBRR_VALUE; 
}

uint8_t *rx_serial(uint8_t limit)
{
  uart_init(false);
  while(sz <= limit)
  {
    while(!(UCSR0A & (1 << RXC0)));
    buffer[sz] = UDR0;
    sz++;
  }
  return (uint8_t *)buffer;
}

uint8_t read_eeprom(uint8_t add)
{
  
  while(EECR & (1 << EEPE));
  EEAR = add;
  EECR |= (1 << EERE);
  eeprom_val = EEDR;
  return eeprom_val;
 
}

void setup_read()
{
  Serial.begin(9600);
  uint8_t start_add = 0x60;
  for(int i =0 ; i <=10; i++)
  {
     Serial.println(read_eeprom(start_add++));
  }   
}


void setup() {
  // put your setup code here, to run once:
   cli();
   sz = 0;
   pinMode(13, OUTPUT);
   delay(5000);              

   digitalWrite(13, LOW);
   
   while(1)
   {
      rx_serial(10);
      if(*buffer != 0x7E)
      {
           continue;
      }else
      {
         uint8_t start_add = 0x60;
         for(int i =0 ; i <=10; i++)
         {
            write_eeprom(buffer[i], start_add++);
         }   
         digitalWrite(13, HIGH);
         break;
      } 
      
   }    

}

void loop() {
  // put your main code here, to run repeatedly:

}
