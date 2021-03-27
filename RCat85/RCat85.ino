/*
 Name:		RCat85.ino
 Created:	3/25/2021 8:55:18 PM
 Author:	Rob Antonisse

 Diverse testen met attiny85, uiteindelijk doel een RC receiver maken met attiny en NRF24

 */


 //#include "SPI.h"
 //#include "RF24.h"

#include <SPI.h>
//#include <RF24_config.h>
#include <RF24.h>
//#include <printf.h>
//#include <nRF24L01.h>

RF24 radio(10,4); //nieuwe pinnen, CE CSN, constructor class RF24 
byte address[][6] = { "TXDCC","RD000" };
byte RX_data[4]; //array pointer ernaar wordt telkens gebruikt dus &RX_data
byte SFT_data=0x0;
unsigned long ledtimer;


// the setup function runs once when you press reset or power the board
void setup() {
DDRB |= (1 << 3);
PORTB &= ~(1 << 3);

	radio.begin();
	radio.setPALevel(RF24_PA_LOW);
	radio.setPayloadSize(4); //max te ontvangen bytes
	radio.openReadingPipe(1, address[0]); // using pipe 1, adress 0 (TX adres hoop ik....)	
	radio.startListening(); // put radio in RX mode
	//radio.setAutoAck(true);
	SFT_spi();
}

// the loop function runs over and over again until power down or reset


void loop() {

	uint8_t pipe;

if (radio.available(&pipe)) {// is there a payload? get the pipe number that recieved it
			radio.read(&RX_data, 4);
			//PINB |= (1 << 3);
			
			SFT_data = 0;
			for (byte i = 0; i < 4; i++) {

				if (RX_data[i] > 0)SFT_data |= (1 << i);
			}
			SFT_spi();
			//SFT_exe();
		}

	//if (millis() - ledtimer > 100) {
	//	ledtimer = millis();
	//		}
}
void SFT_spi() {

	//radio.stopListening();
	//SPI.begin();
	//SPI.beginTransaction(SPISettings(9600, MSBFIRST, SPI_MODE0));
	SPI.transfer(SFT_data);
	PORTB |= (1 << 3);
	//delay(5);
	PORTB &= ~(1 << 3);
	//SPI.endTransaction();	
}

void SFT_exe() {/*
	Rclk=PB3
	SRCLK=PB2
	SERin=PB1	
	CSN van NRF=PB4
	*/
	PORTB &= ~(1 << 4); //csn NRF24 (chip select NOT)
	PORTB &= ~(1 << 3); 
	PORTB &= ~(1 << 2);

	for (byte i = 0; i < 8; i++) {
		PORTB &= ~(1 << 1);	
		if (SFT_data & (1 << i))PORTB |= (1 << 1);
		PINB |= (1 << 2); PINB |= (1 << 2);
	}
	PINB |= (1 << 3); 
	delay(20);
	PINB |= (1 << 3);

	if (SFT_data > 0)PORTB |= (1 << 3);

	//PORTB &= ~(1 << 4); //csn NRF24 (chip select NOT)
	//shift out
}
