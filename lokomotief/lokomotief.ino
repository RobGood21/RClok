/*
 Name:		Lokomotief.ino
 Created:	3/22/2021 5:52:00 PM
 Author:	Rob Antonisse

Diverse ontwikkeling sketch Lokomotief,
Besturing via RC control, power uit lion accu

29mrt2021
Shiftregister instellen via SPI

*/

#include <SPI.h>

//Timer1 gebruiken (attiny heeft die ook, alleen afwijkende prescaler)

//declaraties
byte Shift_byte = 0;
//byte drive; //0=stop 1=Forward 2=reverse
byte fase;
byte dir;

unsigned long tt;
byte extraPrescaler;

byte inPar;
byte inVal;
byte inByte;
byte countRX;

byte duty = 250; //hoe breed is de puls
byte freq = 250;
; //hoe vaak komt de puls, startmoment

//byte parameter;
//byte waarde;

void setup() {
	Serial.begin(9600);

	SER_write(0xFF, 0x00); //send wakeup call

	SPI.begin();

	DDRB |= (1 << 1);// Pin9 as output SS van HC595
	PORTB &= ~(1 << 1); //clear Pin9
	shift(); //clear outputs shiftregister

	//Timer2 instellen,	8 bits timer as in Attiny
	TCCR2A = 0;
	TCCR2A |= (1 << 1);//CTC mode set WGM21

	TCCR2B |= (3 << 0); //prescaler(max)

	OCR2A = 5; //tijdsduur 0-OCR2A

	TIMSK2 |= (1 << 1);//enable timer2_compa interrupt	

}

ISR(TIMER2_COMPA_vect) {
	//TCNT2 = 0x01;
	extraPrescaler++;
	if (extraPrescaler > 50) {
		
		extraPrescaler = 0;
		//Shift_byte ^= (1 << 1);

		switch (fase) {
		case 0:
			Shift_byte &= ~(1 << 1);
			OCR2A = freq; //pauze tussen pulsen
			//Serial.print("*");
			fase = 1;
			break;
		case 1:
			Shift_byte |= (1 << 1);
			OCR2A = duty; //duur puls
			//Serial.print("-");
			fase = 0;
			break;
		}
		shift();
	}
}


void loop() {
	SER_read();
	if (millis() - tt > 1000) {
		tt = millis();
		//parameter++;
		//waarde = waarde + 5;
		//SER_rx();
		//SER_write(parameter, waarde);

	}

}

void shift() {
	SPI.transfer(Shift_byte);
	PINB |= (1 << 1); PINB |= (1 << 1); //make RCLK puls, latch
}

void SER_write(byte para, byte value) {
	Serial.write(0xFF); //Serial.write(state);
	Serial.write(para);
	Serial.write(value);
}
void SER_read() {
	while (Serial.available() > 0) {
		inByte = Serial.read();
		if (inByte == 0xFF) {
			countRX = 1;

		}
		else {
			switch (countRX) {
			case 0: //doe niks start ontvangst 
				break;
			case 1: //ontvang parameter max 254
				inPar = inByte;
				countRX++;
				break;
			case 2: //ontvang value max 254
				inVal = inByte;
				countRX = 0;
				SER_rx();
				break;
			}
		}
	}

}
void SER_rx() {
	//verwerkt ontvangen  boodschap
	//SH_data = random(0, 255);//inVal;
	//shift();
	switch (inPar) {
	case 10: //Motor duty, breedte van de puls
		freq = inVal;
		break;
	case 20: //Motor freq, tijd tussen puls uit en puls aan. 
		duty = inVal;

		break;
	case 30: //Motor richting 0=Stop 1=forward 2= reversed ? of anders

		break;
	}

	SER_write(inPar, inVal); //terug bouncen naar Processing	

		//Serial.print("ontvangen: parameter "); Serial.print(inPar); Serial.print(" Value: "); Serial.println(inVal);

}