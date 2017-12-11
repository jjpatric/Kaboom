#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include "finalheader.h"


void setup(){
	init();
	Serial.begin(9600);
	Serial3.begin(9600);
	tft.begin();
	pinMode(JOY_SEL, INPUT_PULLUP);
	tft.setRotation(3); // rotates to proper orientation (270 degrees CW from default)
	tft.setTextWrap(false);
	tft.fillScreen(GREEN); // fill background
	tft.fillRect(0, 0, DISPLAY_WIDTH, 48, GREY); // cover top portion of screen
}






int main(){
	setup();
	bool startval = startmenu();

	tft.fillRect(0, 48, DISPLAY_WIDTH, DISPLAY_HEIGHT-48, GREEN); // covers menu

	if(startval == 0){
		modeSingle();
	}
	else{
		modeVs();
	}

	return 0;
}
