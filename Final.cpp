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
#define PLAYER_WIDTH 24
#define PLAYER_HEIGHT 32


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
	tft.print("KABOOM!!");

	//initally highlights 1 player
	tft.setCursor(90, 90);
	tft.setTextColor(WHITE, MAGENTA);
	tft.setTextSize(2);
	tft.print("1 Player Mode");

	tft.setCursor(90, 120);
	tft.setTextColor(WHITE, GREEN);
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

int playerX = DISPLAY_WIDTH/2 - PLAYER_WIDTH/2;
int bomberX = DISPLAY_WIDTH/2 - PLAYER_WIDTH/2;

void updatePlayer(){
	int Yvalue = analogRead(JOY_VERT_ANALOG);
	int Xvalue = analogRead(JOY_HORIZ_ANALOG);
	if (Xvalue < JOY_CENTRE - JOY_DEADZONE) {
		playerX += 1;
		playerX = constrain(playerX, 0, DISPLAY_WIDTH - PLAYER_WIDTH);
		tft.fillRect(playerX, DISPLAY_HEIGHT - 46,
	              PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLUE);
		tft.fillRect(playerX - 1, DISPLAY_HEIGHT - 46,
	              2, PLAYER_HEIGHT, ILI9341_GREEN);
	}
	else if (Xvalue > JOY_CENTRE + JOY_DEADZONE) {
		playerX -= 1;
		playerX = constrain(playerX, 0, DISPLAY_WIDTH - PLAYER_WIDTH);
		tft.fillRect(playerX, DISPLAY_HEIGHT - 46,
								PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLUE);
		tft.fillRect(playerX + PLAYER_WIDTH + 1, DISPLAY_HEIGHT - 46,
								2, PLAYER_HEIGHT, ILI9341_GREEN);

	}

}

int timer = 0;
int dir = 1;
void updateBomber(){
	if(timer == 0){ timer = rand()/200; }
	else{ timer--; }
	if(timer == 0 || bomberX == 0 || (bomberX == DISPLAY_WIDTH - PLAYER_WIDTH)){
		dir = -dir;
	}
	bomberX += dir;
	bomberX = constrain(bomberX, 0, DISPLAY_WIDTH - PLAYER_WIDTH);
	tft.fillRect(bomberX, 16,
							PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLACK);
	tft.fillRect(bomberX + PLAYER_WIDTH + 1, 16,
							1, PLAYER_HEIGHT, GREY);
	tft.fillRect(bomberX - 1, 16,
							1, PLAYER_HEIGHT, GREY);
}

#define NUM_BOMBS 100
#define BOMB_SIZE 9
int bombRate = 100;
int bombX[NUM_BOMBS];
int bombY[NUM_BOMBS];
bool bombLife[NUM_BOMBS];
int bombTime = bombRate;
int bombCount = 0;

bool updateBombs(){
	int over = false;
	bombTime--;
	if(bombTime == 0){
		bombX[bombCount] = (bomberX + 4);
		bombY[bombCount] = 48;
		bombLife[bombCount] = true;
		bombCount++;
		bombTime = bombRate;
	}
	for(int i = 0; i < bombCount; i++){
		if(bombLife[i]){
			bombY[i]++;
			tft.fillRect(bombX[i], bombY[i],
		              BOMB_SIZE, BOMB_SIZE, ILI9341_BLACK);
			tft.fillRect(bombX[i], bombY[i]-1,
			            BOMB_SIZE, 1, ILI9341_GREEN);

			if((bombY[i] >= DISPLAY_HEIGHT - 46 - BOMB_SIZE)&&
			(bombX[i] >= playerX - BOMB_SIZE && bombX[i] <= playerX+PLAYER_WIDTH)){
				bombLife[i] = false;
				tft.fillRect(bombX[i], bombY[i],
			              BOMB_SIZE, BOMB_SIZE, ILI9341_GREEN);
			}
		}


	}
	if(bombCount == NUM_BOMBS-1){
		return over;
	}
	else{
		return over;
	}

}

void modeSingle(){
	bool gameOver = false;
	tft.fillRect(playerX, DISPLAY_HEIGHT - 46,
              PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLUE);
	tft.fillRect(bomberX, 16,
							PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLACK);

	while(true){
		updatePlayer();
		updateBomber();
		gameOver = updateBombs();
		delay(1);
		if(gameOver){
			break;
		}
	}

	// game over if bomb position is below player

}

int main(){
	setup();
	bool startval = startmenu();

	tft.fillRect(0, 48, DISPLAY_WIDTH, DISPLAY_HEIGHT-48, GREEN); // covers menu

	if(startval == 0){
		modeSingle();
	}
	else{
		//modeVs();
	}


	return 0;
}
