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

uint8_t read_eeprom(uint8_t add)
{
  
  while(EECR & (1 << EEPE));
  EEAR = add;
  EECR |= (1 << EERE);
  eeprom_val = EEDR;
  return eeprom_val;
  
  
}

void send(uint8_t ch)
{
  while(! (UCSR0A & (1 << UDRE0))) ;
  UDR0 = ch;
}

void tx_serial(const uint8_t * ptr, uint8_t sz){
  uart_init(true);
  while(sz--)
  {
    send(*ptr);
    ptr++;
  }
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


//uint8_t arr[8] = {0x7E, 0x00, 0x04, 0x08, 0x52, 0x4D, 0x59, 0xFF};
// 7E 00 04 08 01 4D 59 50
uint8_t arr[5] = {0x41, 0x54, 0x4D, 0x59, 0x0D};
volatile uint8_t s_val =0 ;

uint8_t rx_serial()
{
  //uart_init(false);
  while(!(UCSR0A & (1 << RXC0)));
  s_val = UDR0;
  return s_val;  
  
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

void setup9()
{
    delay(5000);
    Serial.begin(9600);
   // write_eeprom('G', 0x5f);
    eeprom_val = 0;
    //read_eeprom(0x5f);
    //Serial.println(eeprom_val, HEX); 
    //read_eeprom(0x60);
   // Serial.println(eeprom_val, HEX); 
   
    read_eeprom(0x61);
    Serial.println(eeprom_val, HEX); 

    read_eeprom(0x62);
    Serial.println(eeprom_val, HEX); 

    //read_eeprom(0x63);
    //Serial.println(eeprom_val, HEX); 

    //read_eeprom(0x64);
    //Serial.println(eeprom_val, HEX); 
  
}


void setup() {
   cli();
   //Serial.begin(9600);
   pinMode(13, OUTPUT);
   delay(5000);              
   digitalWrite(13, LOW);
   //delay(5000); 
   
  // tx_serial(arr, 5);
   
   //sz = 0 ;
  // rx_serial(8);
  // Serial.begin(9600);
   
//   uart_init(true);

  // send(0x2B);
  // send(0x2B);
  // send(0x2B);
  
 //  delay(100);
 //  uart_init(false);
   
  // rx_serial();
   //Serial.begin(9600);
   //Serial.println(s_val);
  // write_eeprom(s_val, 0x5f);

  // rx_serial();
  // write_eeprom(s_val, 0x60);

   
   uart_init(true);
   
   send(0x7E);
   send(0x00);
   send(0x04);
   send(0x08);
   send(0x01);
   send(0x4D);
   send(0x59);
   send(0x50);

   digitalWrite(13, HIGH);
   delay(100);
   uart_init(false);
   
   digitalWrite(13, LOW);
   
  // while(s--)
 // {
 //   send(*ptr);
    //Serial.println(*ptr);
   // ptr++;
 // }
 
   //uart_init(false);
   
  // sz =0;

    rx_serial(); 
    rx_serial();
    rx_serial();
    rx_serial();
    rx_serial();
    rx_serial();
    rx_serial();
    rx_serial();

    rx_serial();
    write_eeprom(s_val, 0x61);

    rx_serial();
    write_eeprom(s_val, 0x62);

  // rx_serial();
  // write_eeprom(s_val, 0x63);

 //  rx_serial();
 //  write_eeprom(s_val, 0x64);

   digitalWrite(13, HIGH);

   
   
   //eeprom_val = 0;
   //read_eeprom(0x5f);

   //Serial.begin(9600);
   //Serial.println(s_val);
   
  // Serial.println(UBRR_VALUE);
   
   // Serial.println("HELLO");
   //UCSR0B = ~(1 << TXEN0) & ~(1 << RXEN0);
   // UDR0 = 0x00;
   //uint8_t v =  (0x08 + 0x52 + 0x4D + 0x59) & 0xFF ;
   //uint8_t v1 = 0xFF - v ;
   //Serial.begin(9600);
   //Serial.flush();
   //for(int i = 0 ; i < UART_SENDER_BUFFER_SZ ; i++)
   //{
     // Serial.println(i, HEX);
    // Serial.println(buffer[i], HEX); 
   //}
    //Serial.println(buffer[0], HEX);  
}

void loop() {
  // put your main code here, to run repeatedly:

}


