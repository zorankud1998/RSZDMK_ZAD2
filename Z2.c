#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define USART_RX_BUFFER_SIZE 64

char Rx_Buffer[USART_RX_BUFFER_SIZE];			
volatile unsigned char Rx_Buffer_Size = 0;	
volatile unsigned char Rx_Buffer_First = 0;
volatile unsigned char Rx_Buffer_Last = 0;

ISR(USART_RX_vect)
{
  	Rx_Buffer[Rx_Buffer_Last++] = UDR0;		
	Rx_Buffer_Last &= USART_RX_BUFFER_SIZE - 1;	
	if (Rx_Buffer_Size < USART_RX_BUFFER_SIZE)
		Rx_Buffer_Size++;					
}

void usartInit(unsigned long baud)
{
	UCSR0A = 0x00;	
					
					
	UCSR0B = 0x98;	
					
					

	UCSR0C = 0x06;	
					
					
					

	UBRR0 = F_CPU / (16 * baud) - 1;

	sei();	
}

unsigned char usartAvailable()
{
	return Rx_Buffer_Size;		
}

void usartPutChar(char c)
{
	while(!(UCSR0A & 0x20));	
	UDR0 = c;					
}

void usartPutString(char *s)
{
	while(*s != 0)				
	{
		usartPutChar(*s);		
		s++;					
	}
}

void usartPutString_P(const char *s)
{
	while (1)
	{
		char c = pgm_read_byte(s++);	
		if (c == '\0')					
			return;						
		usartPutChar(c);			
	}
}

char usartGetChar()
{
	char c;

	if (!Rx_Buffer_Size)						
		return -1;
  	c = Rx_Buffer[Rx_Buffer_First++];			
	Rx_Buffer_First &= USART_RX_BUFFER_SIZE - 1;	
	Rx_Buffer_Size--;							

	return c;
}

unsigned char usartGetString(char *s)
{
	unsigned char len = 0;

	while(Rx_Buffer_Size) 			
		s[len++] = usartGetChar();	

	s[len] = 0;						
	return len;						
}


#define BR_KORISNIKA 10

char KORISNICI[BR_KORISNIKA][32] =
{
	"Marko Petrovic",
    "David Silva",
    "Karadjordje Petrovic",
    "Zoka Bosanac",
    "Boris Tadic",
  	"Sloba Milosevic",
  	"Nikola Tesla",
  	"Petar Kralj",
  	"Srecko Sojic",
    "Tiha Spic"
};
char PIN[BR_KORISNIKA][5] = 
{
  "0000",
  "0001",
  "0002",
  "0003",
  "0004",
  "5000",
  "6000",
  "7000",
  "8000",
  "9000"
};

int main()
{
  	char ime_prezime[32];
  	char faza = 0;
  	char broj_korisnika;
  	char in_pin[5];
  	char brojac_f3 = 0;
	usartInit(9600);  	
  	usartPutString("Unesite ime i prezime!\r\n");
	while(1)
      
	{
      	if(usartAvailable() != 0 && faza == 0)
        {
          	_delay_ms(50);
        	usartGetString(ime_prezime);
          	faza = 1;
        }
      	if(faza == 1)
        {
        	for(int i = 0 ; i < BR_KORISNIKA ; i++)
            {
              	char ip_baza[32];
              	strcpy(ip_baza , KORISNICI[i]);
            	if(strcmp(ime_prezime,ip_baza) == 0)
                {
                	broj_korisnika = i;
                  	faza = 2;
                }
                if(i == BR_KORISNIKA - 1 && faza == 1)
              	{
                	usartPutString("Korisnik nije u bazi podataka.\r\n");
			faza = 0;
                }
            }
        }
      	if(faza == 2)
      	{
      		usartPutString("Zdravo ");
          	usartPutString(KORISNICI[broj_korisnika]);
          	usartPutString(" , unesite PIN! \r\n");
          	faza = 3;
      	}
      	if(faza == 3 && brojac_f3 < 4)
        {
        	while(usartAvailable() == 0);
          	_delay_ms(20);
          	in_pin[brojac_f3] = usartGetChar();
          	usartPutChar('*');
          	brojac_f3++;
          	if(brojac_f3 == 4)
            {
              usartPutChar('\r\n');
              faza = 4;
              brojac_f3 == 0;
            }
        }
      	if(faza == 4)
       	{
          	char pinb[5];
          	strcpy(pinb , PIN[broj_korisnika]);
       		if(strcmp(pinb , in_pin) == 0)
            {
            	usartPutString("PIN je ispravno unet!\r\n");
              	faza = 5;
            }
          else 
          	{
            	usartPutString("Neispravan PIN, pokusajte opet!");
            	faza = 3;
          	}
        }
	}

	return 0;
}
