/*
 Name:		RCat85.ino
 Created:	3/25/2021 8:55:18 PM
 Author:	Rob Antonisse

 Diverse testen met attiny85, uiteindelijk doel een RC receiver maken met attiny en NRF24
 
 */

unsigned long ledtimer;


// the setup function runs once when you press reset or power the board
void setup() {
DDRB |= (1 << 3);
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - ledtimer>1000) {
		ledtimer = millis();
		PINB |= (1 << 3);
	}
  
}
