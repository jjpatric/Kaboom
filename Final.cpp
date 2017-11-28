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





void setup(){
init();
Serial.begin(9600);
	tft.begin();
	pinMode(JOY_SEL, INPUT);
	tft.setRotation(3);
	tft.setTextWrap(false);
tft.fillScreen(GREEN);

tft.fillRect(0, 0, DISPLAY_WIDTH, 48, GREY);

}




int main(){
setup();




	return 0;
}
