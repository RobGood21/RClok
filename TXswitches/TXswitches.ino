/*
 Name:		TXswitches.ino
 Created:	3/23/2021 10:09:44 AM
 Author:	Rob Antonisse

 test sketch voor NRF24

 Deze sketch wordt uiteindelijk de RC TX-transmitter, beginnen met een xtal switches
 veel uitleg al weggehaald vanwege leesbaarheid

*/


//libraries
#include <SPI.h>
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>

//declarations RF24
RF24 radio(8, 10); //Constructor class RF24 


// Let these addresses be used for the pair
//uint8_t address[][6] = { "1Node", "2Node" }; //origineel
byte address[][6] = { "TXDCC","RD000" }; //5 bytes adres, char worden omgezet in nummer
/*
TXDCC=(decimaal) 84-88-68-67-67-0 (laatste byte blijft 0)
RD000= 82-68-48-48-48 merk op 0 = dus 48 B0110 0000
5 bytes wat je invoert maakt geen ruk uit als maar 5 bytes en TX en RX moeten beide adressen goed hebben, immers de RX moet weten waar de aknowlidge
Kan zeker ook in loop worden ingevoerd, hoeft niet als constante

*/

byte radioNumber; // Indentificatie van de node?
float payload = 0.0;
byte TX_data[4]; //Wordt naar verwezen met een pointer &TX_data in de RF24 class

unsigned long testtimer;
unsigned long SW_timer;
byte SW_status; // = 0xFF; //switches zijn low-active

void setup() {
	Serial.begin(9600);
	//while (!Serial) {
		// some boards need to wait to ensure access to serial over USB
	//}
	//voor TXswitches
	//ports
	PORTC |= (15 << 0); //pupp-up to pins A0~A3


	if (!radio.begin()) { //start radio, if ok returns
		Serial.println(F("radio hardware is not responding!!"));
		while (1) {} // hold in infinite loop
	}
	Serial.println(F("Jo!... ik ben de TX zender"));
	radioNumber = 0; //indentificatie nummer????

	Serial.print(F("radioNumber = "));
	Serial.println((int)radioNumber);

	
	Serial.print("adres node 0: ");
	for (byte i = 0; i < 6; i++) {
		Serial.print("-");
		Serial.print(address[0][i]);
	}
	Serial.println("");

	Serial.print("adres node 1: ");
	for (byte i = 0; i < 6; i++) {
		Serial.print("-");
		Serial.print(address[1][i]);
	}
	Serial.println("");


	radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default. **Sterkte van de zender PA_MIN is de laagste)

	//radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes
	//** Dit is belangrijk, bepaalt het ,ax aantal bytes wat kan worden gezonden.
	//**Als je hier niks op geeft worden er altijd 32 bytes verzonden, dit geeft natuurlijk een tijdverlies
	//**dus voor deze sketch:
	radio.setPayloadSize(4); 
	
	radio.openWritingPipe(address[radioNumber]);     // hier worden de adressen gekoppeld aan de beide pijpjes,0=TX
	//radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1, RX ? niet nodig in de TX versie
	radio.stopListening();  // put radio in TX mode, doet het ook zonder deze regel nog ff uitzoeken wat dit is.

	SW_status = PINC; //zorgt ervoor dat er geen eerste switch event is by powerup

} // setup

void loop() {
	//altijd TX 
	//if (millis() - testtimer > 5000) {
	//	testtimer = millis();
	//	sendtest();
	//}

	if (millis() - SW_timer > 50) {
		SW_timer = millis();
		SW_exe();
	}



} // loop
void SW_exe() {
	byte css = PINC;
	byte changed = css ^ SW_status;

	if (changed > 0) {
		Serial.print("TX_data : ");
		for (byte i = 0; i < 4; i++) {

			if (changed & (1 << i)) {
				if (css & (1 << i)) {
					TX_data[i] = 0;
				}
				else {
					TX_data[i] = 0xFF;
				}
			}
			Serial.print(TX_data[i]); Serial.print(" ");
		}
		Serial.println("");
		TX_exe();
	}
	SW_status = css;
}
void sendtest() {

	unsigned long start_timer = micros();                    // start the timer
	bool report = radio.write(&payload, sizeof(float));      // transmit & save the report
	//**als RX ontvangst van de payload geeft is report true


	unsigned long end_timer = micros();                      // end the timer

	if (report) { //**dus als report is true alles ok
		Serial.print(F("Transmission successful! "));          // payload was delivered
		Serial.print(F("Time to transmit = "));
		Serial.print(end_timer - start_timer);                 // print the timer result
		Serial.print(F(" us. Sent: "));
		Serial.println(payload);                               // print payload sent
		payload += 0.01;                                       // increment float payload
	}
	else {
		Serial.println(F("Transmission failed or timed out")); // payload was not delivered
	}

}
void TX_exe() {
	//makes tx
	bool report = radio.write(&TX_data, 4);
	if (report) {
		Serial.print(" Ok!");
	}
	else {
		Serial.println("helaas.....niet gelukt");
	}

}
