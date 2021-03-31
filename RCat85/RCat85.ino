/*
 Name:		RCat85.ino
 Created:	3/25/2021 8:55:18 PM
 Author:	Rob Antonisse

Sketch voor testen met een NRF24L01 mini smd aangestuurd door een Attiny85
Attiny op 8Mhz internal.
Zie document Attiny85; Arduino as ISP (drive) voor meer info.
Alle onnodig regels zijn hier verwijderd.

 */
#include <SPI.h>
#include <RF24.h>

RF24 radio(10, 4); //CE CSN, constructor class RF24 (pin 10 bestaat niet, Ce pin NRF24 altijd aan VCC) CSN=SlaveSelect van NRF24
//byte address[][6] = { "TXDCC","RD000" }; //methode als ook een retour adres nodig is. bi-directioneel.
byte testadres[6] = { "TXDCC" }; //Adres array van 6 bytes, bij TX en RX hetzelfde. (letters worden omgezet in ascii)
byte RX_data[4]; //Array voor 4 bytes te ontvangen data
byte SFT_data = 0x0; //byte wat met shiftregister 8 extra pinnen geeft

void setup() {
	//*******NRF24L01
	radio.begin(); //open class RF27 
	radio.setChannel(50);//instellen freq, kanaal. (1~125 opties)
	radio.setPALevel(RF24_PA_LOW);//power level(en dus range) van de Transmitter
	radio.setPayloadSize(4); //max te ontvangen bytes. Anders altijd 32 bytes. Dit versnelt de boel.
	radio.openReadingPipe(1, testadres); //open RX kenmerk pipe 1 adres testadres, alleen tx met dit adres worden verwerkt.
	radio.startListening(); //start proces
	//radio.setAutoAck(true); //staat altijd aan, standaard
	//*****SN74HC595
	DDRB |= (1 << 3); //ss van shiftregister, RCLK
	SFT_spi(); //shift SPI, functie start, clears shiftregister pins 0~7
}
void loop() {
	byte pipeVanDeZender;
	if (radio.available(&pipeVanDeZender)) {// is data ontvanegn? Van welke zender (pipe max 6 mogelijk)
		radio.read(&RX_data, 4); //plaats de ontvangen data in array RX_data (4 bytes)
		SFT_data = 0; //clear en vul het uit te shiften byte
		for (byte i = 0; i < 4; i++) { 
			if (RX_data[i] > 0)SFT_data |= (1 << i);
		}
		SFT_spi();
	}
}
void SFT_spi() {
	SPI.transfer(SFT_data); //maakt gebruik van de SPI library
	PORTB |= (1 << 3); //latch SFT_data in het shiftregister
	PORTB &= ~(1 << 3);
}


