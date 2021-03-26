/*
 Name:		RC4Leds.ino
 Created:	3/22/2021 9:32:03 PM
 Author:	rob antonisse

 Test met NRF24L01 mini smd 
 Doelstelling: snappen hoe het werkt.

 TX node voorzien van 4 drukknoppen
 RX node voorzien van 4 leds.

 4 bytes versturen. 1=true 0=false

 RX leds op pinnen 7,6,5,4
 TX switches op A0,A1,A2,A3
 2 versies maken of alles in een combineren??


 Deze versie alleen ontvangen dus RX.... en de naam


*/

#include <SPI.h>
//#include <RF24_config.h>
#include <RF24.h>
//#include <printf.h>
//#include <nRF24L01.h>


// instantiate an object for the nRF24L01 transceiver
//RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin (AANGEPAST**)

//****************************
RF24 radio(8, 10); //nieuwe pinnen, constructor class RF24 


// Let these addresses be used for the pair
//uint8_t address[][6] = { "1Node", "2Node" };
//uint8_t address[][6] = { "Rober", "Jopie" };
byte address[][6] = { "TXDCC","RD000" };

byte radioNumber = true; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
//**hier werd gebruikt bool radionumber =1 , veranderd in byte, is gewoon een boolean met waarde true

// Used to control whether this node is sending or receiving
bool role = false;  // true = TX role, false = RX role
//***ook weereen boolean waarde false


// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
//float payload = 0.0;
/*
Payload is willekeurig gekozen array.
De class gebruikt de pointer naar deze array. dus &payload of &robbiesdata.....
Voor zenden wordt daar de data gehaald, write bij lezen read wordt de data daar geplaatst. Verwarrend is dezelfde naam....
in deze exaple, maar dat is omdat ze TX en RX in 1 sketch hebben gestopt. 
Dus we gaan werken met TX_data en RX_data 
*/

//declarations RC4leds
byte RX_data[4]; //array pointer ernaar wordt telkens gebruikt dus &RX_data
byte ledstatus=0;
unsigned long ledtimer;

void setup() {	
	
	//Serial.begin(115200);
	Serial.begin(9600);

	while (!Serial) {
		// some boards need to wait to ensure access to serial over USB
	}

	//RC4leds
	//ports
	DDRD |= (240<<0); //set pins 7,6,5,4 as outputs
	//****radio.begin start de boel op en returns een true als gelukt. 
	if (!radio.begin()) {
		Serial.println(F("radio hardware is not responding!!"));
		while (1) {} // hold in infinite loop
	}
	Serial.println(F("Jo!... Ik ga aan de gang"));
	Serial.print(F("radioNumber = "));
	Serial.println((int)radioNumber);

	// role variable is hardcoded to RX behavior, inform the user of this
	//*****hier wordt bedoeld dat de receiver NIET hoeft te woden bevestigd, begint altijd als receiver....
	//*****dit is alleen een mededeling, omschakelen en lezen van serial gebeurt in de loop
	//Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
	// Set the PA Level low to try preventing power supply related problems
	// because these examples are likely run with nodes in close proximity to
	// each other.
	radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default. **Sterkte van de zender PA_MIN is de laagste)

	// save on transmission time by setting the radio to only transmit the
	// number of bytes we need to transmit a float
	radio.setPayloadSize(4); //max te ontvangen bytes	
	radio.openReadingPipe(1, address[0]); // using pipe 1, adress 0 (adres van de TX)
		radio.startListening(); // put radio in RX mode
} // setup
void loop() {
	// This device is a RX node
		uint8_t pipe;
		if (radio.available(&pipe)) {// is there a payload? get the pipe number that recieved it
			radio.read(&RX_data, 4);

			Serial.print("Ontvangen: ");
			ledstatus = 0;
			for (byte i = 0; i < 4; i++) {
				if (RX_data[i] > 0)ledstatus |= (1 << (7-i)); //leds zitten voor de test op d7,d6,d5,d4
				Serial.print(RX_data[i]);
				Serial.print(" ");
			}
			Serial.println("");
			showleds();
		}

} // loop

void showleds() {

	for (byte i = 7; i > 3; i--) {
		//PORTD &=~(120 << 0); //leds off

		if (ledstatus & (1 << i)) {
			PORTD |= (1 << i);
		}
		else {
			PORTD &= ~(1 << i);
		}
	}
}
