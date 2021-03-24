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
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>


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
float payload = 0.0;
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


	// initialize the transceiver on the SPI bus
	//****radio.begin start de boel op en returns een true als gelukt. 

	if (!radio.begin()) {
		Serial.println(F("radio hardware is not responding!!"));
		while (1) {} // hold in infinite loop
	}


	// print example's introductory prompt
	//***gewoon een tekstje
	Serial.println(F("Jo!... Ik ga aan de gang"));

	// To set the radioNumber via the Serial monitor on startup

	//Serial.println(F("Which radio is this? Enter '0' or '1'. Defaults to '0'"));
	//while (!Serial.available()) {
		// wait for user input// oneindigelus, gaat dus niet automatisch verder input vereist
	//}

	//char input = Serial.parseInt();

	//radioNumber = input == 1;

	//***hier wordt de serial monitor schoongeveegd? Waarom.... ?

//****teksjes...
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

	/* ***********************************************************************
	Hier worden de adressen van de TX en RX bepaald, nu 22mrt nog beetje vaag voor me
	Beste hier later mee te spelen, ook om te kunnen zenden andere boodschappen naar verschillen
	Rx nodes (nodes? noemen ze die dingen)
	*/

	// set the TX address of the RX node into the TX pipe
	//radio.openWritingPipe(address[radioNumber]);     // always uses pipe 0

	// set the RX address of the TX node into a RX pipe
	radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

	// additional setup specific to the node's role
	//***role is boolean gedefinieerd als false, dus hier wordt de RX van een module automatisch bij powerup
	//** gestart

		radio.startListening(); // put radio in RX mode



	// For debugging info
	// printf_begin();             // needed only once for printing details
	// radio.printDetails();       // (smaller) function that prints raw register values
	// radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup


//*** tot hier 22mrt2021


void loop() {

	/*
	if (millis() - ledtimer > 1000) { //looplichie		
		ledtimer = millis();
	if (ledstatus == 0) ledstatus = 128;
	showleds();
//Serial.println(ledstatus);
	ledstatus = (ledstatus >> 1);
	}		
*/
	// This device is a RX node
		uint8_t pipe;
		if (radio.available(&pipe)) {// is there a payload? get the pipe number that recieved it
			/*
			Merk op wordt iedere loop gedaan.... dus heel snel proces
			radio.available(&pipe)  returns a true als er iets ontvangen is, en de variabele pipe het pipe nummer,
			Van de ontvanger
			*/
			//uint8_t bytes = radio.getPayloadSize(); // get the size of the payload

			//radio.read(&payload, 4);            // fetch payload from FIFO
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
