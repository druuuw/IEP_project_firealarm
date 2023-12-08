/*
HOW TO USE THIS THING:

if there is fire (>30 deg), alarm will auto trigger. press RIGHT button OR 3 on remote to call 995 and activate SOS. to STOP SOS, press LEFT button OR 5 on remote to go back to alarm

to trigger alarm manually, press RIGHT button OR press 2 on remote

to STOP alarm if triggered manually, press LEFT button OR 1 on remote

to trigger SOS manually regardless of alarm, press both LEFT and RIGHT buttons together OR press 4 on remote

to sub to tenz press 6

to change alarm sound, press 7, 8, or 9

*/

#include <RichShieldVoltageSensor.h>
#include <Wire.h>
#include "RichShieldPassiveBuzzer.h"
#include <RichShieldIRremote.h>
#define rxpin 2
IRrecv IR(rxpin);


#include <RichShieldKEY.h>
#include <RichShieldKnob.h>
#include <RichShieldLED.h>
#include <RichShieldLightSensor.h>
#include "RichShieldNTC.h"
#define NTC_PIN A1
NTC temper(NTC_PIN);

#include "RichShieldTM1637.h"
#define CLK 10
#define DIO 11
TM1637 disp(CLK, DIO);

#define PassiveBuzzerPin 3
PassiveBuzzer buz(PassiveBuzzerPin);

#include "RichShieldDHT.h"
DHT dht;

#define YELLOW_LED 7
#define GREEN_LED 6
#define BLUE_LED 5
#define RED_LED 4
#define BUTTON1 8
#define BUTTON2 9
int alarm_status = 0;
int alarm_choice = 1;
int rxout;
void alarm1(void);
void alarm2(void);
void alarm3(void);
int remote(void);
void tuturu(void);
void scdf(void);
int8_t fire[4] = { 15,1,4,14 }; //for displaying "FIRE” in form of “F14E”

void setup() {
	IR.enableIRIn();
	disp.init();
	delay(1000);
	disp.display(0, 18); //startup display “HI”
	disp.display(1, 1);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(BUTTON1, INPUT_PULLUP);
	pinMode(BUTTON2, INPUT_PULLUP);
	tuturu(); //startup sound
}


void loop() {
	float celsius;
	celsius = dht.readTemperature(); //read temp
	displayTemperature((int8_t)celsius); //call function to display temp
	if (digitalRead(BUTTON1) == 0 || celsius > 35 || remote() == 2) //set alarm status to 1
	{
		alarm_status = 1;
		rxout = 0;
	}
	if (alarm_status == 1)
	{
		switch (alarm_choice) {
		case 1: alarm1(); //trigger alarm with alarm1 sound
			break;
		case 2: alarm2(); //trigger alarm with alarm2 sound
			break;
		case 3: alarm3(); //trigger alarm with alarm3 sound
			break;
		default: break;
		}
	}
	if ((digitalRead(BUTTON1) == 0 && digitalRead(BUTTON2) == 0) || remote() == 4) //manual trigger SOS
	{
		scdf();
		rxout = 0;
	}

}

void alarm1(void) {
	for (; ;) {
		disp.display(fire);
		digitalWrite(YELLOW_LED, HIGH);
		buz.playTone(2000, 100);
		buz.playTone(1300, 100);
		digitalWrite(YELLOW_LED, LOW);
		digitalWrite(RED_LED, HIGH);
		buz.playTone(2000, 100);
		buz.playTone(1300, 100);
		digitalWrite(RED_LED, LOW);
		if (digitalRead(BUTTON2) == 0 || remote() == 1) { //off alarm
			alarm_status = 0;
			rxout = 0;
			break;
		}
		if (digitalRead(BUTTON1) == 0 || remote() == 3) { //press button1 again or IR button 3 to simulate call 995
			scdf();
			rxout = 0;
		}
	}
	return;
}

void alarm2(void) {
	for (;;) {
		disp.display(fire);
		digitalWrite(YELLOW_LED, HIGH);
		buz.playTone(2400, 300);
		digitalWrite(YELLOW_LED, LOW);
		digitalWrite(RED_LED, HIGH);
		buz.playTone(2400, 300);
		digitalWrite(RED_LED, LOW);
		if (digitalRead(BUTTON2) == 0 || remote() == 1) { //off alarm
			alarm_status = 0;
			rxout = 0;
			break;
		}
		if (digitalRead(BUTTON1) == 0 || remote() == 3) { //press button1 again or IR button 3 to simulate call 995
			scdf();
			rxout = 0;
		}
	}
	return;

}

void alarm3(void) {
	int i;
	for (;;) {
		disp.display(fire);
		for (i = 2000; i <= 2500; i += 80) {
			digitalWrite(YELLOW_LED, HIGH);
			buz.playTone(i, 70);
			digitalWrite(YELLOW_LED, LOW);
			delay(10);
		}
		for (; i >= 2000; i -= 80) {
			digitalWrite(RED_LED, HIGH);
			buz.playTone(i, 70);
			digitalWrite(RED_LED, LOW);
			delay(10);
		}
		if (digitalRead(BUTTON2) == 0 || remote() == 1) { //off alarm
			alarm_status = 0;
			rxout = 0;
			break;
		}
		if (digitalRead(BUTTON1) == 0 || remote() == 3) { //press button1 again or IR button 3 to simulate call 995
			scdf();
			rxout = 0;
		}
	}

}

void displayTemperature(int8_t temperature)
{
	int8_t temp[4];
	if (temperature < 0)
	{
		temp[0] = INDEX_NEGATIVE_SIGN;
		temperature = abs(temperature);
	}
	else if (temperature < 100)temp[0] = INDEX_BLANK;
	else temp[0] = temperature / 100;
	temperature %= 100;
	temp[1] = temperature / 10;
	temp[2] = temperature % 10;
	temp[3] = 12;
	disp.display(temp); //display temp
}

int remote(void)
{
	//set remote value back to default (0)
	if (IR.decode()) {
		if (IR.isReleased())

			buz.playTone(2500, 50); //beep

		switch (IR.keycode)
		{
		case KEY_ZERO: rxout = 0; break; //default
		case KEY_ONE: rxout = 1; break; //manual override OFF alarm
		case KEY_TWO: rxout = 2; break; //manual override ON alarm 
		case KEY_THREE: rxout = 3; break; //simulate call 995 when alarm on
		case KEY_FOUR: rxout = 4; break; //simulate call 995 regardless of alarm status
		case KEY_FIVE: rxout = 5; break; //manual override SOS OFF
		case KEY_SIX: tuturu(); break; //play startup sound again
		case KEY_SEVEN: alarm_choice = 1; break; //set alarm sound to alarm1
		case KEY_EIGHT: alarm_choice = 2; break; //set alarm sound to alarm2
		case KEY_NINE: alarm_choice = 3; break; //set alarm sound to alarm3
		default: break;
		}
	}
	IR.resume(); //Receive the next value
	return rxout; //returns the value of button pressed
}
void scdf(void)
{
	int8_t scdf[4] = { 9,9,5,INDEX_BLANK }, sos[4] = { 5,0,5, INDEX_BLANK };
	int i;
	for (i = 0; i < 3; i++) //loop 3 times
	{
		disp.display(scdf); //995
		delay(1000);
		disp.display(sos); //SOS
		buz.playTone(550, 1000); //morse code (S)OS
		disp.display(scdf); //995
		delay(500);
		buz.playTone(550, 1000);
		disp.display(sos); //SOS
		delay(500);
		buz.playTone(550, 1000);
		delay(1000);

		if (digitalRead(BUTTON2) == 0 || remote() == 5) // off SOS and return to prev state
		{
			return;
		}
		disp.display(scdf); //995
		buz.playTone(550, 2000); //morse code S(O)S
		disp.display(sos); //SOS
		delay(500);
		buz.playTone(550, 2000);
		disp.display(scdf); //995
		delay(500);
		buz.playTone(550, 2000);
		delay(1000);

		if (digitalRead(BUTTON2) == 0 || remote() == 5) // off SOS and return to prev state
		{
			return;
		}
		disp.display(sos); //SOS
		buz.playTone(550, 1000); //morse code SO(S)
		disp.display(scdf); //995
		delay(500);
		buz.playTone(550, 1000);
		disp.display(sos); //SOS
		delay(500);
		buz.playTone(550, 1000);
		disp.display(scdf); //995
		delay(1000);
	}
	return;
}

void tuturu(void)
{
	buz.playTone(2000, 20); //congrats on your tenz sub
	buz.playTone(700, 80);
	delay(200);
	buz.playTone(2000, 20);
	buz.playTone(300, 100);
	buz.playTone(700, 550);
	return;
}









