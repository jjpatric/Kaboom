#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include "lcd_image.h"

//function for the main menu. using the joystick, when 1 player is selected,
//returns 0. When Head to head is selected, returns 1
bool startmenu(){
	bool value = 0;
	tft.setCursor(90, 50);
	tft.setTextColor(RED);
	tft.setTextSize(3);
	tft.setTextWrap(false);
	tft.print("KABOOM!!");


	//initally highlights 1 player
	tft.setCursor(90, 90);
	tft.setTextColor(WHITE, MAGENTA);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("1 Player Mode");

	tft.setCursor(90, 120);
	tft.setTextColor(WHITE, GREEN);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("Head To Head!!");

	while (true){
		int Yvalue = analogRead(JOY_VERT_ANALOG);
		Serial.println(Yvalue);

		if (Yvalue > JOY_CENTRE + JOY_DEADZONE) {
			tft.setCursor(90, 90);
			tft.setTextColor(WHITE, GREEN);
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("1 Player Mode");

			tft.setCursor(90, 120);
			tft.setTextColor(WHITE, MAGENTA);
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("Head To Head!!");
			value = 1;
		}

		if (Yvalue < JOY_CENTRE - JOY_DEADZONE) {

			tft.setCursor(90, 90);
			tft.setTextColor(WHITE, MAGENTA);
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("1 Player Mode");

			tft.setCursor(90, 120);
			tft.setTextColor(WHITE, GREEN);
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("Head To Head!!");
			value = 0;
		}

	}
	return 1;
}
