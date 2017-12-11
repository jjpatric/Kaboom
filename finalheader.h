#ifndef _FINAL_HEADER
#define _FINAL_HEADER


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


int playerX = DISPLAY_WIDTH/2 - PLAYER_WIDTH/2; // initial player position, middle of display
int bomberX = DISPLAY_WIDTH/2 - PLAYER_WIDTH/2; // initial bomber position

int timer = 0; // time before bomber switches direction
int dir = -1; // current bomber direction

#define NUM_BOMBS 1000 // number of bombs the bomber will drop
#define BOMB_SIZE 9 // size of sides of bombs
int bombRate = 25; // rate at which bombs are spawned
int bombX[NUM_BOMBS]; // bombs x position
int bombY[NUM_BOMBS]; // bombs y position
bool bombLife[NUM_BOMBS]; // true if bomb is in play false  if otherwize
int bombTime = bombRate; // timer that counts down till next bomb drop
int bombCount = 0; // number of bombs that have been dropped so far
uint16_t score = 0; // player score
int dlay = 7; // starting delay between updates
int deadBombs = 0; // counts number of bombs caught in order to reduce loop time for high bomb counts
bool first = true; // checks it is the first bomb caughts


bool startmenu(){
	bool value = 0;
	// title
	tft.setCursor(90, 50);
	tft.setTextColor(RED);
	tft.setTextSize(3);
	tft.print("KABOOM!!");
	// instructions
	tft.setCursor(90, 150);
	tft.setTextSize(1);
	tft.print("Press Joystick to select");

	//initally highlights 1 player
	tft.setCursor(90, 90);
	tft.setTextColor(WHITE, MAGENTA);
	tft.setTextSize(2);
	tft.print("1 Player Mode");

	tft.setCursor(90, 120);
	tft.setTextColor(WHITE, GREEN);
	tft.print("Head To Head!!");

	while (true){
		int Yvalue = analogRead(JOY_VERT_ANALOG); // reads joystick value
		if(digitalRead(JOY_SEL) == LOW){ // if player chooses an option
			break;
		}

		if (Yvalue > JOY_CENTRE + JOY_DEADZONE) { // player presses down
			tft.setCursor(90, 90);
			tft.setTextColor(WHITE, GREEN); // deselect '1 Player'
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("1 Player Mode");

			tft.setCursor(90, 120);
			tft.setTextColor(WHITE, MAGENTA); // select '2 Player'
			tft.print("Head To Head!!");
			value = 1;
		}

		if (Yvalue < JOY_CENTRE - JOY_DEADZONE) { // player presses up
			tft.setCursor(90, 90);
			tft.setTextColor(WHITE, MAGENTA);// select '1 Player'
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("1 Player Mode");

			tft.setCursor(90, 120);
			tft.setTextColor(WHITE, GREEN); // deselect '2 Player'
			tft.setTextSize(2);
			tft.setTextWrap(false);
			tft.print("Head To Head!!");
			value = 0;
		}

	}
	return value;
}


void updatePlayer(){
	int Xvalue = analogRead(JOY_HORIZ_ANALOG); // reads joystick x-values
	if (Xvalue < JOY_CENTRE - JOY_DEADZONE) { // if player pushes right
		playerX += 1; //move right
		playerX = constrain(playerX, 0, DISPLAY_WIDTH - PLAYER_WIDTH); // within display
		tft.fillRect(playerX + PLAYER_WIDTH-1, DISPLAY_HEIGHT - 46, // draw one line in direction of movement
	              1, PLAYER_HEIGHT, ILI9341_BLUE);
		tft.fillRect(playerX - 1, DISPLAY_HEIGHT - 46, // covers previous position in 1 pixel wide line of background color
	              1, PLAYER_HEIGHT, ILI9341_GREEN);
	}
	else if (Xvalue > JOY_CENTRE + JOY_DEADZONE) { // if player pushes left
		playerX -= 1; //move left
		playerX = constrain(playerX, 0, DISPLAY_WIDTH - PLAYER_WIDTH); // within display
		tft.fillRect(playerX, DISPLAY_HEIGHT - 46, // similar to above
								1, PLAYER_HEIGHT, ILI9341_BLUE);
		tft.fillRect(playerX + PLAYER_WIDTH + 1, DISPLAY_HEIGHT - 46, // similar to above
								1, PLAYER_HEIGHT, ILI9341_GREEN);

	}

}


void updateBomber(){
	if(timer == 0){ timer = rand()/analogRead(13); } // sets random time before bomber turns around
	else{ timer--; } // counts down timer
	while(timer > 3000){
		timer = rand()/analogRead(13);
	}
	if(timer == 0 || bomberX == 0 || (bomberX == DISPLAY_WIDTH - PLAYER_WIDTH)){
		dir = -dir; // switches bomber direction
	}
	bomberX += dir; // moves bomber in current direction
	bomberX = constrain(bomberX, 0, DISPLAY_WIDTH - PLAYER_WIDTH); // constrains bomber to screen
	tft.fillRect(bomberX, 20, // draws left most line of bomber
							1, PLAYER_HEIGHT-4, ILI9341_BLACK);
	tft.fillRect(bomberX + PLAYER_WIDTH, 20, // draws rightmost line of bomber
							1, PLAYER_HEIGHT-4, ILI9341_BLACK);
	tft.fillRect(bomberX + PLAYER_WIDTH + 1, 20, // draws background over right side
							1, PLAYER_HEIGHT-4, GREY);
	tft.fillRect(bomberX - 1, 20, // draws background over left side
							1, PLAYER_HEIGHT-4, GREY);
}


int updateBombs(){
	int over = 0; // game over false as default
	bombTime--; // decrease time till next bomb dropped
	if(bombTime == 0){ // time to drop a bomb
		bombX[bombCount] = (bomberX + 4); // starting position under middle of bomber
		bombY[bombCount] = 48;
		bombLife[bombCount] = true; // sets this bombs life to true, how inspiring
		tft.fillRect(bombX[bombCount], bombY[bombCount], // draws initial bomb position
		              BOMB_SIZE, BOMB_SIZE, ILI9341_BLACK);
		bombCount++; // increases count of how moany bombs have been dropped
		bombTime = bombRate; // resets bomb timer
	}
	for(int i = deadBombs; i < bombCount; i++){ // goes through all living bombs
		if(bombLife[i]){ // if it's still alive
			bombY[i]++; // moves bomb position one pixel down
			tft.fillRect(bombX[i], bombY[i] + BOMB_SIZE - 1,
		              BOMB_SIZE, 1, ILI9341_BLACK); // draws one line in next row of pixels under bomb(BLACK)
			tft.fillRect(bombX[i], bombY[i]-1, // draws one line in previous row of pixels above bomb(GREEN)
			            BOMB_SIZE, 1, ILI9341_GREEN);

			if((bombY[i] >= DISPLAY_HEIGHT - 46 - BOMB_SIZE)&& // checks if bomb is in contact with player
			(bombX[i] >= playerX - BOMB_SIZE && bombX[i] <= playerX+PLAYER_WIDTH)){
				bombLife[i] = false; // bomb dies :(
				deadBombs++; // add to the graveyard of bombs used to limit for loop
				if(first){ // bug fix catching a bomb that wasn't the lowest prematurely killed a bomb
					deadBombs --;
					first = false; // no longer first bomb to die
				}
				tft.fillRect(bombX[i], bombY[i], // makes bomb look like it dissapeared
			              BOMB_SIZE, BOMB_SIZE, ILI9341_GREEN);
				score += 10; // increase score by 10
				tft.fillRect(200, 5, // covers previous score
			              DISPLAY_WIDTH-200, 14, GREY);
				tft.setCursor(200, 5);
				tft.setTextColor(RED);
				tft.setTextSize(2);
				tft.print(score); // prints new score
				if(bombCount%100 == 99){ // every 99th bomb
					if(dlay > 0){ 				 // where dlay can't go below 0
						dlay -= 1;          // decrease delay between updates
					}
				}
			}
			else if(bombY[i] > DISPLAY_HEIGHT-12){ // if bomb gets too low
				over = 1;                        	   // GAME OVER
			}
			else if(bombCount == 990){
				over = 2;
			}
		}
	}
	return over; // returns whether game over or not
}

int playGame(){
	int gameOver = 0;
	tft.fillRect(playerX, DISPLAY_HEIGHT - 46, // draws initial player position
							PLAYER_WIDTH, PLAYER_HEIGHT, ILI9341_BLUE);
	tft.fillRect(bomberX, 20, // draws initial bomber position
							PLAYER_WIDTH, PLAYER_HEIGHT-4, ILI9341_BLACK);
	tft.setCursor(200, 5);
	tft.setTextColor(RED);
	tft.setTextSize(2);
	tft.print(score); // draws initial score

	while(true){ // while game is playing keep updating all characters
		updatePlayer();
		updateBomber();
		gameOver = updateBombs();
		delay(dlay); // creates feeling of increasing speed with smaller 'dlay'
		if(gameOver == 1 || gameOver == 2){
			return gameOver;
		}
	}
}

void modeSingle(){
	int gameOutcome = playGame();
	if(gameOutcome == 1){
		for(int j = 0; j < 3; j++){ // *explosion effects*
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_YELLOW);
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_ORANGE);
		}
		// prints out all end screen text

		tft.setCursor(40, 50);
		tft.setTextColor(RED);
		tft.setTextSize(5);
		tft.print("KABOOM!!");

		tft.setCursor(90, 130);
		tft.setTextSize(3);
		tft.print("GAME OVER");

		tft.setCursor(40, 100);
		tft.print("Score: ");
		tft.setCursor(150, 100);
		tft.print(score);

		tft.setCursor(40, 190);
		tft.setTextColor(MAGENTA);
		tft.setTextSize(1);
		tft.print("Press RESET To Play Again");

		while(true){} // wait till player gathers the guts to play again
	}
	else if(gameOutcome == 2){
		for(int j = 0; j < 3; j++){ // *explosion effects*
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_YELLOW);
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_GREEN);
		}
		tft.setCursor(40, 50);
		tft.setTextColor(RED);
		tft.setTextSize(5);
		tft.print("KABOOM!!");

		tft.setCursor(90, 130);
		tft.setTextSize(3);
		tft.print("YOU WON!");

		tft.setCursor(40, 100);
		tft.print("Score: ");
		tft.setCursor(150, 100);
		tft.print(score);

		tft.setCursor(40, 190);
		tft.setTextColor(MAGENTA);
		tft.setTextSize(1);
		tft.print("Press RESET To Play Again");
	}
}

void uint16_to_serial3(uint16_t num) {
  Serial3.write((char) (num >> 0));
  Serial3.write((char) (num >> 8));
}

uint16_t uint16_from_serial3() {
  uint16_t num = 0;
  num = num | ((uint16_t) Serial3.read()) << 0;
  num = num | ((uint16_t) Serial3.read()) << 8;
  return num;
}

void modeVs(){
	tft.setCursor(10, 70);
	tft.setTextColor(RED);
	tft.setTextSize(2);
	tft.print("Waiting on other player...");
	tft.setCursor(10, 100);
	tft.print("Game will begin with them");



	Serial3.write(1);
	while(Serial3.available() == 0){}
	Serial3.read();

	tft.fillRect(0, 48, DISPLAY_WIDTH, DISPLAY_HEIGHT-48, GREEN); // covers text

	int gameOutcome = playGame();
	if(gameOutcome == 1){
		for(int j = 0; j < 3; j++){ // *explosion effects*
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_YELLOW);
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_ORANGE);
		}
		// prints out all end screen text

		tft.setCursor(40, 50);
		tft.setTextColor(RED);
		tft.setTextSize(5);
		tft.print("KABOOM!!");

		tft.setCursor(90, 130);
		tft.setTextSize(3);
		tft.print("GAME OVER");

		tft.setCursor(40, 100);
		tft.print("Score: ");
		tft.setCursor(150, 100);
		tft.print(score);

		tft.setCursor(40, 190);
		tft.setTextColor(MAGENTA);
		tft.setTextSize(1);
		tft.print("Waiting for other player to finish...");


		uint16_to_serial3(score);
		while(Serial3.available() == 0){}
		uint16_t otherScore = uint16_from_serial3();
		Serial.println(otherScore);


		if(otherScore > score){ // loser
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_ORANGE);
			tft.setCursor(40, 50);
			tft.setTextColor(RED);
			tft.setTextSize(5);
			tft.print("YOU LOSE");

			tft.setTextSize(2);
			tft.setCursor(10, 100);
			tft.print("Your Score: ");
			tft.setCursor(150, 100);
			tft.print(score);
			tft.setCursor(10, 130);
			tft.print("Opponent's Score: ");
			tft.setCursor(220, 130);
			tft.print(otherScore);

			tft.setCursor(40, 190);
			tft.setTextColor(MAGENTA);
			tft.setTextSize(1);
			tft.print("Press RESET To Play Again");
		}
		else if(otherScore < score){ // winner
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_GREEN);
			tft.setCursor(40, 50);
			tft.setTextColor(RED);
			tft.setTextSize(5);
			tft.print("YOU WIN");

			tft.setTextSize(2);
			tft.setCursor(10, 100);
			tft.print("Your Score: ");
			tft.setCursor(150, 100);
			tft.print(score);
			tft.setCursor(10, 130);
			tft.print("Opponent's Score: ");
			tft.setCursor(220, 130);
			tft.print(otherScore);

			tft.setCursor(40, 190);
			tft.setTextColor(MAGENTA);
			tft.setTextSize(1);
			tft.print("Press RESET To Play Again");
		}
		else if(otherScore == score){ // tie-er
			tft.fillRect(0, 0,
									DISPLAY_WIDTH, DISPLAY_HEIGHT, ILI9341_BLUE);
			tft.setCursor(40, 50);
			tft.setTextColor(RED);
			tft.setTextSize(5);
			tft.print("YOU TIED!");

			tft.setTextSize(2);
			tft.setCursor(10, 100);
			tft.print("Your Score: ");
			tft.setCursor(150, 100);
			tft.print(score);

			tft.setCursor(40, 190);
			tft.setTextColor(MAGENTA);
			tft.setTextSize(1);
			tft.print("Press RESET To Play Again");
		}
		while(true){} // wait till player gathers the guts to play again
	}




}


#endif
