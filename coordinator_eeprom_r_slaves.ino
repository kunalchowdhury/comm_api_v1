#include <avr/io.h>

#define F_CPU_1 16000000
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU_1 / (USART_BAUDRATE * 16UL))) - 1)
#define UART_SENDER_BUFFER_SZ 0x1F
#define REMOTE_AT_COMMAND_SIZE 0x13
#define REMOTE_AT_COMMAND_RESP_SIZE 0x15
#define REMOTE_AT_COMMAND_ACK_FAILURE 0x13
#define ZIGBEE_TX_REQUEST_SIZE 0x1A
#define SERIAL_REQ_TIMEOUT_VAL 1000

uint8_t buffer[UART_SENDER_BUFFER_SZ];
volatile uint8_t sz = 0;


struct destination_address_64{
  uint8_t address[8];
};

struct destination_address_16{
  uint8_t address[2];
};

struct rf_data{
  uint8_t data[8];
};

struct tx_req_data
{
  uint8_t tx_data[UART_SENDER_BUFFER_SZ];
};


struct zigbee_transmit_request{
  struct destination_address_64 address_64;
  struct rf_data data ;
  struct tx_req_data tx_data;
  struct  destination_address_16 address_16;
  uint8_t start_delimiter;
  uint8_t msb;
  uint8_t lsb;
  uint8_t frame_type;
  uint8_t frame_id;
  uint8_t broadcast_radius;
  uint8_t options;
  uint8_t checksum;
  
};


struct zigbee_transmit_request zb_tx_req_arduino;
struct zigbee_transmit_request *p_zb_tx_req_arduino = &zb_tx_req_arduino;

void init_zb_tx_request(struct zigbee_transmit_request *p_zb_tx_req, uint8_t msg_length, uint8_t frame_type){
  p_zb_tx_req->start_delimiter = 0x7E;
  p_zb_tx_req->msb = 0x00;
  p_zb_tx_req->lsb = msg_length; //0x16;
  p_zb_tx_req->frame_type= frame_type ; //0x10;
  p_zb_tx_req->broadcast_radius = 0x00;
}

uint8_t * serial_payload(struct zigbee_transmit_request * ptr, boolean is_slave_query)
{
  uint8_t checksum = 0;
  ptr->tx_data.tx_data[0x00] = ptr->start_delimiter ;
  ptr->tx_data.tx_data[0x01] = ptr->msb ;
  ptr->tx_data.tx_data[0x02] = ptr->lsb ;
  
  ptr->tx_data.tx_data[0x03] = ptr->frame_type ; 
  checksum += ptr->tx_data.tx_data[0x03] ;
  
  ptr->tx_data.tx_data[0x04] = ptr->frame_id ;
  checksum += ptr->tx_data.tx_data[0x04] ;
  
  for (uint8_t c = 0x05 ; c < 0x0D ; c++)
  {
    ptr->tx_data.tx_data[c] = ptr->address_64.address[0x07-(c-0x05)];
    checksum += ptr->tx_data.tx_data[c]  ;
  }
  for (uint8_t c = 0x0D ; c < 0x0F ; c++)
  {
    ptr->tx_data.tx_data[c] = ptr->address_16.address[0x01-(c-0x0D)];
    checksum += ptr->tx_data.tx_data[c]  ;
  
  }

  if(is_slave_query)
  {
      ptr->tx_data.tx_data[0x0F] = 0x02;
      checksum += ptr->tx_data.tx_data[0x0F];


      ptr->tx_data.tx_data[0x10] = 0x4D;
      checksum += ptr->tx_data.tx_data[0x10];

      
      ptr->tx_data.tx_data[0x11] = 0x59;
      checksum += ptr->tx_data.tx_data[0x11];
      
      checksum =  checksum & 0xFF ;

      ptr->tx_data.tx_data[0x12] = 0xFF - checksum;
  }
  else 
  {
      ptr->tx_data.tx_data[0x0F] = 0x00;
      checksum += ptr->tx_data.tx_data[0x0F]  ;
      
      ptr->tx_data.tx_data[0x10] = 0x00;
      checksum += ptr->tx_data.tx_data[0x10]  ;
      
      for (uint8_t c = 0x11 ; c < 0x19 ; c++)
      {
        ptr->tx_data.tx_data[c] = ptr->data.data[0x07-(c-0x11)];
        checksum += ptr->tx_data.tx_data[c]  ;
      }
    
      checksum =  checksum & 0xFF ;
      ptr->tx_data.tx_data[0x19] = 0xFF - checksum;
  }
  return ptr->tx_data.tx_data ;
}
void clear_buffer()
{
     uint8_t ct = UART_SENDER_BUFFER_SZ;
     while(ct--)
     {
       buffer[ct] = 0; 
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

volatile uint8_t s_val =0 ;
bool rx_serial()
{
  uint16_t ctr = 1;
  while(!(UCSR0A & (1 << RXC0)));
  s_val = UDR0;
  return true;  
}

void receive()
{
  sz =0;
  uart_init(false);
  rx_serial();
  buffer[sz++] = s_val;
  
  rx_serial();
  uint8_t msb = s_val;
  buffer[sz++] = msb;

  rx_serial();
  uint8_t lsb = s_val;
  buffer[sz++] = lsb;

  for(int i = 0 ; i <= (lsb - msb) ; i++)
  {
    rx_serial();
    buffer[sz++] = s_val;
  }
}

void rx_serial(uint8_t limit)
{
  sz =0;
  uart_init(false);
  while(limit--)
  {
    if(rx_serial())
    {
      buffer[sz] = s_val;
      sz++;
    }
    else 
    {
      break ; // dont try post timeout 
    }
  }
}
void write_eeprom(uint8_t c, uint8_t add)
{
   while(EECR & (1 << EEPE)); 
   EEAR = add;
   EEDR = c;
   EECR |= (1 << EEMPE);
   EECR |= (1 << EEPE);
 }
volatile uint8_t trial ; 
uint8_t discovered_slave(uint8_t * slave_add)
{
   //digitalWrite(13, HIGH);
   cli();
   init_zb_tx_request(p_zb_tx_req_arduino, 0x0F, 0x17);
   p_zb_tx_req_arduino->frame_id = 0x01;
  
   for(int i =7 ; i >= 0 ; i--)
   {
     uint8_t val = *slave_add;
     p_zb_tx_req_arduino->address_64.address[i] = val;
     slave_add++; 
   }

   p_zb_tx_req_arduino->address_16.address[1] = 0xFF;
   p_zb_tx_req_arduino->address_16.address[0] = 0xFE;
  
   uint8_t * payload = serial_payload(p_zb_tx_req_arduino, true);
  
   tx_serial(payload, REMOTE_AT_COMMAND_SIZE);
   digitalWrite(13, LOW);
   delay(100);
   clear_buffer();
   receive();
   digitalWrite(13, HIGH);
   bool is_valid = validate_response();

   if(!is_valid)
   { 
      return 0;  
   }
   
//   uint8_t start = 0x72;
//   sz--;
//   for(uint8_t j = 0; j < sz ; j++)
//   {
//     write_eeprom(buffer[j], start++); 
//   }
   //write_eeprom(0xBC, 0x97); 
   if(!(buffer[0x0D] ^ 0xFF) & !(buffer[0x0E] ^ 0xFE))
   {
       return 0;
   }
   
//   write_eeprom(0xBE, 0x50); 
   
   for(int i =7 ; i >= 0 ; i--)
   {
     p_zb_tx_req_arduino->address_64.address[i] = 0;
   }

   return 1;
}

bool validate_response()
{
  if(buffer[0] != 0x7E)
  {
    return false;
  }

  uint8_t len = buffer[2] - buffer [1] ;
  uint8_t i ;
  uint8_t checksum = 0;
  for(i = 3 ; i < len + 3 ; i++)
  {
     checksum  += buffer[i];    
  } 
  checksum = checksum & 0xFF ;
  checksum = 0xFF - checksum;
  return checksum == buffer[len+3] ;
  
}

void send_slave_msg(uint8_t slave_hi, uint8_t slave_lo, uint8_t cmd_type)
{
  init_zb_tx_request(p_zb_tx_req_arduino, 0x16, 0x10);
  p_zb_tx_req_arduino->frame_id = 0x01;
  p_zb_tx_req_arduino->address_64.address[1] = 0xFF;
  p_zb_tx_req_arduino->address_64.address[0] = 0xFF;
  p_zb_tx_req_arduino->address_16.address[1] = slave_hi;
  p_zb_tx_req_arduino->address_16.address[0] = slave_lo;
  p_zb_tx_req_arduino->options = 0x00;
  p_zb_tx_req_arduino->data.data[0] = cmd_type ;
  uint8_t * payload = serial_payload(p_zb_tx_req_arduino, false);
//  uint8_t i = 0x1A;
//  uint8_t start_add = 0x72;
//  while(i--)
//  {
//       write_eeprom(*payload, start_add++);
//       payload++;
//    //  Serial.print(i+" -> "); 
//    //  Serial.println(*payload, HEX);
//    //  payload++;
//    //  i++;
//  }

//....................
  digitalWrite(13, HIGH); 
  tx_serial(payload, ZIGBEE_TX_REQUEST_SIZE);
  delay(100);
  clear_buffer();
  receive();
  digitalWrite(13, LOW);
//  //uint8_t c = REMOTE_AT_COMMAND_RESP_SIZE;
  uint8_t start_add = 0x72;
  sz--;
  write_eeprom(sz, start_add++);
  for(int i = 0 ; i <= sz ; i++)
  {
     write_eeprom(buffer[i], start_add++);  
  }
  
}

volatile uint8_t eeprom_val;

uint8_t read_eeprom(uint8_t add)
{
  
  while(EECR & (1 << EEPE));
  EEAR = add;
  EECR |= (1 << EERE);
  eeprom_val = EEDR;
  return eeprom_val;
 
}

volatile uint8_t start_add = 0x60 ;
volatile uint8_t slave_sz = 0 ;

uint8_t** slaves_16_address;
uint8_t** slaves_64_address;

uint8_t** init_array( uint8_t rows, uint8_t cols)
{
   uint8_t** arr;
   arr = (uint8_t **)malloc(rows * sizeof(uint8_t *));
   for(int i = 0 ; i < rows ; i++)
   {
      arr[i] =  (uint8_t *)malloc(cols * sizeof(uint8_t));
    
   }
   return arr;   
}

void deallocate_array(uint8_t ** arr, int rows)
{
   for(int i = 0; i < rows ; i++)
   {
        free(arr[i]);
     
   }

   free(arr);
}
void setup3()
{
  uint8_t a = 0xFF;
  uint8_t b = 0xFE;
  
  uint8_t c = 0xEA ;//a^b;
  uint8_t d = 0x21;

  uint8_t e = !(c ^ a) & !(d ^ d);
  Serial.begin(9600);
  Serial.println(c, HEX);
  Serial.println(d, HEX);
  Serial.println(e, HEX);
  Serial.println("----");
  for(uint8_t i = 0 ; i < 255; i++)
  {
    Serial.println(!i, HEX);  
  }

  uint8_t v = !(0xFF ^ 0xFF);  
  Serial.println(v, HEX);
}

void setup()
{
 // write_eeprom(0x72, 0);
//  write_eeprom(0x73, 0);
//  Serial.begin(9600);
//  Serial.println("DONE");
   
//   uint8_t s = read_eeprom(0x83);
//   uint8_t s1 = read_eeprom(0x84);
//   Serial.begin(9600);
//   Serial.println(s, HEX);
//   Serial.println(s1, HEX);
//

   uint8_t start = 0x72;
   Serial.begin(9600);
//   uint8_t checksum = 0;
   uint8_t c = 12;//read_eeprom(start++)+1;
   //for(uint8_t j = 0; j <= 0x19 ; j++)
   Serial.println(c);
   Serial.println("-----");
   while(c--)
   {
   //write_eeprom(0, start++);
     uint8_t v = read_eeprom(start++);
     Serial.println(v, HEX);
//     if(j >=3 )
//     {
//       checksum += v;
//     }
//     //write_eeprom(buffer[j], start++); 
   }

Serial.println("....");
   Serial.println(read_eeprom(0x97), HEX);
   Serial.println(read_eeprom(0x50), HEX);
   Serial.println(read_eeprom(0x99), HEX);
//   Serial.println(read_eeprom(0x50), HEX);
//   Serial.println(read_eeprom(0x51), HEX);
////   checksum = checksum & 0xFF ;
//   checksum = 0xFF - checksum ;
//   Serial.println("checksum");
//   Serial.println(checksum, HEX);
//   Serial.println(read_eeprom(start), HEX);
     //Serial.begin(9600);
     //Serial.println(dummy(100));
    

     
}

void setup0()
{
   delay(5000);
   pinMode(13, OUTPUT); 
    
   slave_sz  = read_eeprom(start_add++);
   slaves_16_address = init_array(1, 2);
   slaves_64_address = init_array(1, 8);
   int i , j; 
  
   for(i = 0 ; i < slave_sz; i++)
   {
       for(j = 0 ; j < 8 ; j++)
       {
           *(*(slaves_64_address+i)+j) = read_eeprom(start_add++);
       }  
   }
   while(slave_sz--)
   {
       uint8_t idx = slave_sz ;
       uint8_t v = discovered_slave(slaves_64_address[slave_sz]);
       write_eeprom(v, 0x99);
       if(v)
       {
            
            **(slaves_16_address+idx) = buffer[0x0D];
            *(*(slaves_16_address+idx) +1) = buffer[0x0E];
        
//          slave_sz--;
//          if(slave_sz == 0)
//          {
//              write_eeprom(0x51, 0x51);
//              break;  
//          }
//          
        } 
       delay(5000);
   }
   digitalWrite(13, LOW);
   send_slave_msg(*(*(slaves_16_address)), *(*(slaves_16_address)+1), 0x09);

}


void loop(){}

