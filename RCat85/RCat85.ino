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
unsigned long ledtimer;


// the setup function runs once when you press reset or power the board
void setup() {

	radio.begin();
	radio.setPALevel(RF24_PA_LOW);
	radio.setPayloadSize(4); //max te ontvangen bytes
	radio.openReadingPipe(1, address[0]); // using pipe 1, adress 0 (TX adres hoop ik....)
	radio.startListening(); // put radio in RX mode
	//radio.setAutoAck(true);


	DDRB |= (1 << 3);
}

// the loop function runs over and over again until power down or reset
void loop() {
uint8_t pipe;
		if (radio.available(&pipe)) {// is there a payload? get the pipe number that recieved it
			radio.read(&RX_data, 4);
			PINB |= (1 << 3);
		}

	//if (millis() - ledtimer > 100) {
	//	ledtimer = millis();
	//		}
}
