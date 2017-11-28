#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>


#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240


// joystick pins
#define JOY_VERT_ANALOG A1
#define JOY_HORIZ_ANALOG A0
#define JOY_SEL 2

// constants for the joystick
#define JOY_DEADZONE 64
#define JOY_CENTRE 512
#define JOY_STEPS_PER_PIXEL 64
//some colors for my artiques
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define GREY 0x8410

#define CURSOR_SIZE_HEIGHT 36
#define CURSOR_SIZE_WIDTH 27

int mode = 0;


void setup(){
	init();
	Serial.begin(9600);
	tft.begin();
	pinMode(JOY_SEL, INPUT_PULLUP);
	tft.setRotation(3);
	tft.setTextWrap(false);
	tft.fillScreen(GREEN);
	tft.fillRect(0, 0, DISPLAY_WIDTH, 48, GREY);
}

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
		if(digitalRead(JOY_SEL) == LOW){
			break;
		}

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
	return value;
}

void startgame(){
	tft.fillRect(0, 48, DISPLAY_WIDTH, DISPLAY_HEIGHT-48, GREEN);
}

int main(){
	setup();
	bool startval = startmenu();

	if(startval == 0){
		modeSingle();
	}
	else{
		//modeVs();
	}


	return 0;
}
