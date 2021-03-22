/*
	Name:       TestMetRC.ino
	Created:	22-3-2021 9:22:44
	Author:     Rob Antonisse

	Testen met rc gebruik makend van een NRF24L01 mini smd


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
uint8_t address[][6] = { "1Node", "2Node" };



// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit

byte radioNumber = true; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
//**hier werd gebruikt bool radionumber =1 , veranderd in byte, is gewoon een boolean met waarde true

// Used to control whether this node is sending or receiving
bool role = false;  // true = TX role, false = RX role
//***ook weereen boolean waarde false


// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;

void setup() {
	//Serial.begin(115200);
	Serial.begin(9600);

	while (!Serial) {
		// some boards need to wait to ensure access to serial over USB
	}

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
	Serial.println(F("Which radio is this? Enter '0' or '1'. Defaults to '0'"));
	while (!Serial.available()) {
		// wait for user input// oneindigelus, gaat dus niet automatisch verder input vereist
	}
	char input = Serial.parseInt();

	radioNumber = input == 1;

	//***hier wordt de serial monitor schoongeveegd? Waarom.... ?

//****teksjes...
	Serial.print(F("radioNumber = "));
	Serial.println((int)radioNumber);

	// role variable is hardcoded to RX behavior, inform the user of this
	//*****hier wordt bedoeld dat de receiver NIET hoeft te woden bevestigd, begint altijd als receiver....



	//*****dit is alleen een mededeling, omschakelen en lezen van serial gebeurt in de loop
	Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));


	// Set the PA Level low to try preventing power supply related problems
	// because these examples are likely run with nodes in close proximity to
	// each other.
	radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default. **Sterkte van de zender PA_MIN is de laagste)

	// save on transmission time by setting the radio to only transmit the
	// number of bytes we need to transmit a float
	radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes
	//***Hiermee geef je aan hoeveel bytes moeten worden verzonden
	//Niks invullen geeft 32 bytes, hmmm top!



	/* ***********************************************************************
	Hier worden de adressen van de TX en RX bepaald, nu 22mrt nog beetje vaag voor me
	Beste hier later mee te spelen, ook om te kunnen zenden andere boodschappen naar verschillen
	Rx nodes (nodes? noemen ze die dingen)
	*/

	// set the TX address of the RX node into the TX pipe
	radio.openWritingPipe(address[radioNumber]);     // always uses pipe 0

	// set the RX address of the TX node into a RX pipe
	radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

	// additional setup specific to the node's role
	//***role is boolean gedefinieerd als false, dus hier wordt de RX van een module automatisch bij powerup
	//** gestart
	if (role) {
		radio.stopListening();  // put radio in TX mode
	}
	else {
		radio.startListening(); // put radio in RX mode
	}


	// For debugging info
	// printf_begin();             // needed only once for printing details
	// radio.printDetails();       // (smaller) function that prints raw register values
	// radio.printPrettyDetails(); // (larger) function that prints human readable data

} // setup


//*** tot hier 22mrt2021


void loop() {

	if (role) {
		// This device is a TX node

		unsigned long start_timer = micros();                    // start the timer
		bool report = radio.write(&payload, sizeof(float));      // transmit & save the report
		unsigned long end_timer = micros();                      // end the timer

		if (report) {
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

		// to make this example readable in the serial monitor
		delay(1000);  // slow transmissions down by 1 second

	}
	else {
		// This device is a RX node

		uint8_t pipe;
		if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
			uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
			radio.read(&payload, bytes);            // fetch payload from FIFO
			Serial.print(F("Received "));
			Serial.print(bytes);                    // print the size of the payload
			Serial.print(F(" bytes on pipe "));
			Serial.print(pipe);                     // print the pipe number
			Serial.print(F(": "));
			Serial.println(payload);                // print the payload's value
		}
	} // role

	if (Serial.available()) {
		// change the role via the serial monitor
		//******zit in de loop, dus in runtime is de functie TX/RX te wisselen....




		char c = toupper(Serial.read());
		if (c == 'T' && !role) {
			// Become the TX node

			role = true;
			Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
			radio.stopListening();

		}
		else if (c == 'R' && role) {
			// Become the RX node

			role = false;
			Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
			radio.startListening();
		}
	}

} // loop
