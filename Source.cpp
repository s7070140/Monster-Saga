#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
bool startGame = false;
bool restartGame = false;
int currentTime;

void init();
void loadMedia();
void close();
bool home();
bool game();
bool gameNew();
bool checkOverlap(SDL_Rect, SDL_Rect);
int checkScore(int);
void saveScore(char[31], int, int);

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
TTF_Font* fontScore = NULL;


class Texture //deal with texture
{
public:
	Texture();
	void loadFile(std::string path); //load texture
	void free(); //clear all
	void render(int, int, SDL_Rect* clip = NULL); //render texture
	void loadText(std::string text, SDL_Color textColor, TTF_Font*); //load string into texture
	int get_width(); //return width of texture
	int get_height(); //return height of texture
	int get_x(); //get x position
	int get_y(); //get y position
	SDL_Texture* get_texture(); // get texture
private:
	SDL_Texture* media;
	int width;
	int height;
	int numX;
	int numY;
};
Texture::Texture()
{
	media = NULL;
	width = 0;
	height = 0;
}
void Texture::loadFile(std::string path)
{
	free();
	SDL_Surface* surface = IMG_Load(path.c_str());
	media = SDL_CreateTextureFromSurface(renderer, surface);
	width = surface->w;
	height = surface->h;
	SDL_FreeSurface(surface);
	surface = NULL;
}
void Texture::free()
{
	SDL_DestroyTexture(media);
	media = NULL;
	width = 0;
	height = 0;
	numX = 0;
	numY = 0;
}
void Texture::render(int x, int y, SDL_Rect* clip)
{
	SDL_Rect imgRect = { x, y, width, height };
	if (clip != NULL)
	{
		imgRect.w = clip->w;
		imgRect.h = clip->h;
	}
	numX = x;
	numY = y;
	SDL_RenderCopy(renderer, media, clip, &imgRect);
}
void Texture::loadText(std::string text, SDL_Color textColor, TTF_Font* f)
{
	free();
	SDL_Surface* textSurface = TTF_RenderText_Blended(f, text.c_str(), textColor);
	media = SDL_CreateTextureFromSurface(renderer, textSurface);
	width = textSurface->w;
	height = textSurface->h;
	SDL_FreeSurface(textSurface);
}
int Texture::get_width()
{
	return width;
}
int Texture::get_height()
{
	return height;
}
int Texture::get_x()
{
	return numX;
}
int Texture::get_y()
{
	return numY;
}
SDL_Texture* Texture::get_texture()
{
	return media;
}

Texture background;

class Background //for control background of the game
{
	public:
		Background(); //initialize background
		void move(int, int); //move bg
		void render(); //render bg to screen
		int get_speed(); //return speed of moving
	private:
		int x;
		int speed;
		bool check;
		int current;
};
Background::Background()
{
	x = 0;
	speed = 2;
	check = false;
	current = 0;
}
void Background::move(int time, int count)
{
	if (time % 60 == 0 && !check && time != 0)
	{
		speed++;
		check = true;
		current = time;
	}
	if (time - current >= 1)
		check = false;
	if (speed > 10)
		speed = 10;
    if (count % 20 == 0 && count != 0)
		x -= 1;
	if (x <= -SCREEN_WIDTH - 795)
		x = 0;
}
void Background::render()
{
	background.render(x, 0, NULL);
}
int Background::get_speed()
{
	return speed;
}

Texture monster;
SDL_Rect monClip[8];

class Monster //for control monster
{
	public:
		Monster(); //initialize class
		void eventKey(int); //event
		void move(int); //moving monster
		void render(); //render monster
		SDL_Rect get_rect(); //get current rect
	private:
		int posX;
		int posY;
		int conY;
		int count;
		int current;
		SDL_Rect rect;
};
Monster::Monster()
{
	posX = (SCREEN_WIDTH / 2) / 2;
	posY = SCREEN_HEIGHT / 2;
	conY = 0;
	current = 0;
	rect = { posX, posY, 120, 80 };
}
void Monster::eventKey(int count)
{
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_SPACE] && count % 10 == 0)
		conY -= 2;
	else if (count % 20 == 0)
		conY += 2;
}
void Monster::move(int count)
{
	if (count % 75 == 0)
		current++;
	posY += conY;
	if (posY <= 0 || posY + monster.get_height() >= SCREEN_HEIGHT)
		posY -= conY;
	conY = 0;
	if (current > 7)
		current = 0;
	rect = { posX+50, posY+12, 20, 45 };
}
void Monster::render()
{
	monster.render(posX, posY, &monClip[current]);
}
SDL_Rect Monster::get_rect()
{
	return rect;
}

Texture trunkTop;
Texture trunkLow;
SDL_Rect trunkTopClip[3];
SDL_Rect trunkLowClip[3];

class Object
{
	public:
		Object();
		void move(int, int, SDL_Rect);
		void render();
		bool isCrash();
	private:
		int topA, lowA;
		int topB, lowB;
		int clipTopA;
		int clipLowA;
		int clipTopB;
		int clipLowB;
		bool crash;
		bool checkA, checkB, startB;
};
Object::Object()
{
	topA = SCREEN_WIDTH;
	lowA = SCREEN_WIDTH;
	topB = SCREEN_WIDTH;
	lowB = SCREEN_WIDTH;
	clipTopA = 0;
	clipLowA = 0;
	clipTopB = 0;
	clipLowB = 0;
	crash = false;
	checkA = false;
	checkB = false;
	startB = false;
}
void Object::move(int speed, int count, SDL_Rect monster)
{

	if (!checkA)
	{
		clipTopA = rand() % 2;
		if (clipTopA == 3)
			clipLowA = rand() % 1;
		else
			clipLowA = rand() % 2;
		checkA = true;
	}
	if (!checkB)
	{
		clipTopB = rand() % 2;
		if (clipTopB == 3)
			clipLowB = rand() % 1;
		else
			clipLowB = rand() % 2;
		checkB = true;
	}

	if (topA <= SCREEN_WIDTH / 2)
		startB = true;

	if (count % 20 == 0)
	{
		topA -= speed;
		lowA -= speed;
		if (startB)
		{
			topB -= speed;
			lowB -= speed;
		}
	}

	if (topA + trunkTopClip[clipTopA].w <= 0)
	{
	    if(topB <= SCREEN_WIDTH / 2)
        {
            checkA = false;
            topA = SCREEN_WIDTH;
            lowA = SCREEN_WIDTH;
        }
	}
	if (topB + trunkLowClip[clipTopB].w <= 0)
	{
	    if(topA <= SCREEN_WIDTH / 2)
        {
            checkB = false;
            topB = SCREEN_WIDTH;
            lowB = SCREEN_WIDTH;
        }
	}


	count++;
	SDL_Rect objTopA = { topA, 0, trunkTopClip[clipTopA].w, trunkTopClip[clipTopA].h };
	SDL_Rect objLowA = { lowA, SCREEN_HEIGHT - trunkLowClip[clipLowA].h, trunkLowClip[clipLowA].w, trunkLowClip[clipLowA].h };
	SDL_Rect objTopB = { topB, 0, trunkTopClip[clipTopA].w, trunkTopClip[clipTopB].h };
	SDL_Rect objLowB = { lowB, SCREEN_HEIGHT - trunkLowClip[clipLowB].h, trunkLowClip[clipLowB].w, trunkLowClip[clipLowB].h };

	if (checkOverlap(monster, objTopA))
		crash = true;
	else if (checkOverlap(monster, objLowA))
		crash = true;
	else if (checkOverlap(monster, objTopB))
		crash = true;
	else if (checkOverlap(monster, objLowB))
		crash = true;
}
void Object::render()
{
    trunkTop.render(topA, 0, &trunkTopClip[clipTopA]);
    trunkLow.render(lowA, SCREEN_HEIGHT - trunkLowClip[clipLowA].h, &trunkLowClip[clipLowA]);
    trunkTop.render(topB, 0, &trunkTopClip[clipTopB]);
    trunkLow.render(lowB, SCREEN_HEIGHT - trunkLowClip[clipLowB].h, &trunkLowClip[clipLowB]);
}
bool Object::isCrash()
{
	return crash;
}


Texture text;
Texture press_text;
Texture menuText;
Texture logo;
Texture startButton;
Texture scoreButton;
Texture helpButton;
Texture exitButton;
Texture bgBoard;
Texture helpBoard;
Texture closeButton;
Texture creditBg;
Texture creditButton;
Texture resumeBoard;
Texture resumeButton;
Texture quitButton;
Texture restartButton;
Texture scoreText;
Texture blur;
Texture endBoard;
SDL_Rect buttonClip[3];
SDL_Rect closeClip[3];
SDL_Rect pressRect[5];
SDL_Rect circleButton[3];
Mix_Music* music = NULL;
Mix_Chunk* mouseIn = NULL;
Mix_Chunk* mousePress = NULL;

class Button
{
	public:
		Button();
		void loadButton(Texture);
		int stateButton(SDL_Event*);
		void moveIn();
		void moveOut();
		void render(int);
		void renderClose();
		void renderMenu();
		void renderX(int x);
		void renderForEnd(int x);
	private:
		Texture button;
		int left, right;
		int top, low;
		int appear;
		int state;
		int closeB;
		int numY;
		int endY;
		bool inside_sound;
		bool press_sound;
};
Button::Button()
{
	left = 0, right = 0;
	top = 0, low = 0;
	appear = -400;
	state = 0;
	closeB = 0;
	endY = -resumeButton.get_height() / 3 - 50;
	numY = -resumeButton.get_height() / 3 - 50;
	inside_sound = false;
	press_sound = false;
}
void Button::loadButton(Texture pButton)
{
	button = pButton;
}
int Button::stateButton(SDL_Event *e)
{
	bool inside = true;
	int numberButton = 0;

	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if (mouseX < left || mouseX > right)
			inside = false;
		else if (mouseY < top || mouseY > low)
			inside = false;
	}
	if (!inside)
	{
		state = 0;
		inside_sound = false;
		press_sound = false;
	}
	else
	{
		switch (e->type)
		{
			case SDL_MOUSEMOTION:
			{
				if (!inside_sound)
				{
					Mix_PlayChannel(-1, mouseIn, 0);
					inside_sound = true;
				}
				state = 1;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				state = 2;
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (!press_sound)
				{
					Mix_PlayChannel(-1, mousePress, 0);
					press_sound = true;
				}
				state = 1;
				if (top == 150)
					return 1;
				else if (top == 250)
					return 2;
				else if (top == 350 && left != 275 && left != 425)
					return 3;
				else if (top == 450)
					return 4;
				else if (low == button.get_y() + closeButton.get_height() / 3)
					return 5;
				else if (left == 225)
					return 6;
				else if (left == 350 || left == 275)
					return 7;
				else if (left == 475 || left == 425)
					return 8;
				break;
			}
		}
	}
	return 0;
}
void Button::moveIn()
{
	appear += 5;
	closeB += 5;
	numY += 5;
	endY += 5;
	if (appear > 50)
	{
		appear = 50;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + 75;
	}
	if (closeB > bgBoard.get_width() + button.get_width() / 2)
	{
		closeB = bgBoard.get_width() + button.get_width() / 2;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + closeButton.get_height() / 3;
	}
	if (numY > 300)
	{
		numY = 300;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + button.get_height() / 3;
	}
	if (endY > 350)
	{
		endY = 350;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + button.get_height() / 3;
	}
}
void Button::moveOut()
{
	appear -= 5;
	closeB -= 5;
	numY -= 5;
	endY -= 5;
	if (appear <= -475)
	{
		appear = -475;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + 75;
	}
	if (closeB <= -button.get_width())
	{
		closeB = -button.get_width();
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + closeButton.get_height() / 3;
	}
	if (numY <= -resumeButton.get_height() / 3 - 50)
	{
		numY = -resumeButton.get_height() / 3 - 50;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + button.get_height() / 3;
	}
	if (endY <= -resumeButton.get_height() / 3 - 50)
	{
		endY = -resumeButton.get_height() / 3 - 50;
		left = button.get_x(), right = button.get_x() + button.get_width();
		top = button.get_y(), low = button.get_y() + button.get_height() / 3;
	}
}
void Button::render(int y)
{
	button.render(200, appear + y, &buttonClip[state]);
}
void Button::renderClose()
{
	button.render(closeB - closeButton.get_width(), 100 - (closeButton.get_height() / 3) / 2, &closeClip[state]);
}
void Button::renderMenu()
{
	button.render(300, appear);
}
void Button::renderX(int x)
{
	button.render(x, numY, &circleButton[state]);
}
void Button::renderForEnd(int x)
{
	button.render(x, endY, &circleButton[state]);
}

class Board
{
	public:
		Board();
		void get_board(Texture);
		void moveIn();
		void moveOut();
		void render();
	private:
		Texture board;
		int appear;
};
Board::Board()
{
	appear = 0;
}
void Board::get_board(Texture pBoard)
{
	board = pBoard;
	appear = -board.get_width() - closeButton.get_width() - 15;
}
void Board::moveIn()
{
	appear += 5;
	if (appear >= 0)
		appear = 0;
}
void Board::moveOut()
{
	appear -= 5;
	if (appear <= -board.get_width() - closeButton.get_width() - 15)
		appear = -board.get_width() - closeButton.get_width() - 15;
}
void Board::render()
{
	board.render(appear, 100);
}

class Pause
{
	public:
		Pause();
		void loadBoard(Texture);
		void moveIn();
		void moveOut();
		void render(int);
		int get_width();
		int get_x();
	private:
		Texture board;
		int numY;
};
Pause::Pause()
{
	numY = 0;
}
void Pause::loadBoard(Texture pBoard)
{
	board = pBoard;
	numY = -board.get_height();
}
void Pause::moveIn()
{
	numY += 5;
	if (numY >= SCREEN_HEIGHT / 2 - board.get_height() / 2)
		numY = SCREEN_HEIGHT / 2 - board.get_height() / 2;
}
void Pause::moveOut()
{
	numY -= 5;
	if (numY <= -board.get_height())
		numY = -board.get_height();
}
void Pause::render(int x)
{
	board.render(x, numY);
}
int Pause::get_width()
{
	return board.get_width();
}
int Pause::get_x()
{
	return board.get_x();
}

void init()
{//initialize
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Monster Saga", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	TTF_Init();
}
void loadMedia()
{//load all media
	background.loadFile("Image Stuff/bgForest.png");
	press_text.loadFile("Image Stuff/press_text.png");
	music = Mix_LoadMUS("Sound/nyan_cat.mp3");
	font = TTF_OpenFont("Simply Rounded.ttf", 27);
	logo.loadFile("Image Stuff/logo.png");
	menuText.loadFile("Image Stuff/menu.png");
	startButton.loadFile("Image Stuff/startButton.png");
	scoreButton.loadFile("Image Stuff/scoreButton.png");
	helpButton.loadFile("Image Stuff/helpButton.png");
	exitButton.loadFile("Image Stuff/exitButton.png");
	bgBoard.loadFile("Image Stuff/bgBoard.png");
	closeButton.loadFile("Image Stuff/closeButton.png");
	helpBoard.loadFile("Image Stuff/helpBoard.png");
	creditBg.loadFile("Image Stuff/credit_bg.png");
	creditButton.loadFile("Image Stuff/credit_button.png");
	resumeBoard.loadFile("Image Stuff/bgStopMenu.png");
	resumeButton.loadFile("Image Stuff/resumeButton.png");
	quitButton.loadFile("Image Stuff/returnButton.png");
	restartButton.loadFile("Image Stuff/restartButton.png");
	blur.loadFile("Image Stuff/blurBG.png");
	endBoard.loadFile("Image Stuff/endBoard.png");
	fontScore = TTF_OpenFont("Arial round.ttf", 52);
	mouseIn = Mix_LoadWAV("Sound/01_inside.wav");
	mousePress = Mix_LoadWAV("Sound/02_press.wav");
	monster.loadFile("Image Stuff/monster.png");
	trunkTop.loadFile("Image Stuff/trunkTop.png");
	trunkLow.loadFile("Image Stuff/trunkLow.png");

	trunkTopClip[0] = { 0, 0, 122, 130 };
	trunkTopClip[1] = { 200, 0, 150, 200 };
	trunkTopClip[2] = { 441, 0, 123, 292 };

	trunkLowClip[2] = { 400, 20, 140, 310 };
	trunkLowClip[1] = { 190, 130, 150, 200 };
	trunkLowClip[0] = { 0, 219, 131, 111 };

    int num, i;

	num = 0;
	for (i = 0; i < 5; i++)
	{
		pressRect[i] = { 0, num, 450, 25 };
		num += 25;
	}

	num = 0;
	for (i = 0; i < 3; i++)
	{
		buttonClip[i] = { 0, num, startButton.get_width(), 75 };
		num += 75;
	}

	num = 0;
	for (i = 0; i < 3; i++)
	{
		closeClip[i] = { 0, num, closeButton.get_width(), closeButton.get_height() / 3 };
		num += closeButton.get_height() / 3;
	}

	num = 0;
	for (i = 0; i < 3; i++)
	{
		circleButton[i] = { 0, num, resumeButton.get_width(), resumeButton.get_height() / 3 };
		num += 100;
	}
	num = 0;
	for(i=0; i<8; i++)
    {
        monClip[i] = {num, 0, monster.get_width() / 8, monster.get_height()};
        num += 90;
    }
}
void close()
{//close and clear all
	background.free();
	text.free();
	logo.free();
	menuText.free();
	startButton.free();
	scoreButton.free();
	helpButton.free();
	exitButton.free();
	bgBoard.free();
	closeButton.free();
	helpBoard.free();
	creditBg.free();
	creditButton.free();
	resumeBoard.free();
	quitButton.free();
	resumeButton.free();
	restartButton.free();
	scoreText.free();
	blur.free();
	monster.free();
	trunkLow.free();
	trunkTop.free();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_CloseFont(fontScore);
	Mix_FreeChunk(mouseIn);
	Mix_FreeChunk(mousePress);
	font = NULL;
	fontScore = NULL;
	window = NULL;
	renderer = NULL;
	mouseIn = NULL;
	mousePress = NULL;
	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
	TTF_Quit();
}
bool home()
{
	bool quit = false;
	SDL_Event e;
	const Uint8* key = SDL_GetKeyboardState(NULL);
	SDL_Color color = { 255, 255, 255, 255 };

	Texture player1;
	Texture player2;
	Texture player3;
	Texture player4;
	Texture player5;
	Texture sp1;
	Texture sp2;
	Texture sp3;
	Texture sp4;
	Texture sp5;
	Button menu;
	Button start;
	Button score;
	Button help;
	Button exit;
	Button close;
	Board highScore;
	Board howToPlay;
	menu.loadButton(menuText);
	start.loadButton(startButton);
	score.loadButton(scoreButton);
	help.loadButton(helpButton);
	exit.loadButton(exitButton);
	close.loadButton(closeButton);
	highScore.get_board(bgBoard);
	howToPlay.get_board(helpBoard);

	int count = 0, x = 0, rect = 0, textX = -700, scoreX = -400;
	bool checkScore = false;
	bool checkHelp = false;

	FILE* f;
	int scoreNum[5], i = 0;
	char name[5][40];
	char allName[5][100];
	char allScore[5][100];
	f = fopen("score.txt", "r");
	for (i = 0; i < 5; i++)
		fscanf(f, "%s %d", &name[i], &scoreNum[i]);
	for (i = 0; i < 5; i++)
	{
		sprintf(allName[i], "%d.%-50s", i + 1, name[i]);
		sprintf(allScore[i], "%d", scoreNum[i]);
	}
	fclose(f);

	//load all high score into texture
	player1.loadText(allName[0], color, font);
	player2.loadText(allName[1], color, font);
	player3.loadText(allName[2], color, font);
	player4.loadText(allName[3], color, font);
	player5.loadText(allName[4], color, font);
	sp1.loadText(allScore[0], color, font);
	sp2.loadText(allScore[1], color, font);
	sp3.loadText(allScore[2], color, font);
	sp4.loadText(allScore[3], color, font);
	sp5.loadText(allScore[4], color, font);

	while (!quit)
	{
		currentTime = SDL_GetTicks() / 1000;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return false;
			}
			else if (e.type == SDL_KEYDOWN)
				startGame = true;
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); //reset screen
		SDL_RenderClear(renderer);

		background.render(x, 0, NULL);

		if (!startGame)
		{
			logo.render(SCREEN_WIDTH / 2 - logo.get_width() / 2, (SCREEN_HEIGHT / 2) / 2, NULL);
			press_text.render(SCREEN_WIDTH / 2 - press_text.get_width() / 2, 450, &pressRect[rect]);
		}
		else
		{
			int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0;
			if (!checkScore && !checkHelp)
			{
				num1 = start.stateButton(&e);
				num2 = score.stateButton(&e);
				num3 = help.stateButton(&e);
				num4 = exit.stateButton(&e);

				menu.moveIn();
				start.moveIn();
				score.moveIn();
				help.moveIn();
				exit.moveIn();
				highScore.moveOut();
				close.moveOut();
				howToPlay.moveOut();

				if (textX <= -620)
				{
					textX = -620;
					scoreX = -200;
				}
				else
				{
					textX -= 5;
					scoreX -= 5;
				}

			}
			else if (checkScore && !checkHelp)
			{
				if (key[SDL_SCANCODE_ESCAPE])
				{
					checkScore = false;
					checkHelp = false;
				}
				num5 = close.stateButton(&e);

				menu.moveOut();
				start.moveOut();
				score.moveOut();
				help.moveOut();
				exit.moveOut();
				highScore.moveIn();
				close.moveIn();

				if (textX >= 80)
				{
					textX = 80;
					scoreX = 550;
				}
				else
				{
					textX += 5;
					scoreX += 5;
				}

			}
			else if (!checkScore && checkHelp)
			{
				if (key[SDL_SCANCODE_ESCAPE])
				{
					checkScore = false;
					checkHelp = false;
				}
				num5 = close.stateButton(&e);

				menu.moveOut();
				start.moveOut();
				score.moveOut();
				help.moveOut();
				exit.moveOut();
				howToPlay.moveIn();
				close.moveIn();

				if (textX <= -620)
				{
					textX = -620;
					scoreX = -200;
				}
				else
				{
					textX -= 5;
					scoreX -= 5;
				}

			}

			menu.renderMenu();
			start.render(100);
			score.render(200);
			help.render(300);
			exit.render(400);
			highScore.render();
			howToPlay.render();
			player1.render(textX, 220);
			player2.render(textX, 260);
			player3.render(textX, 300);
			player4.render(textX, 340);
			player5.render(textX, 380);
			sp1.render(scoreX, 220);
			sp2.render(scoreX, 260);
			sp3.render(scoreX, 300);
			sp4.render(scoreX, 340);
			sp5.render(scoreX, 380);
			close.renderClose();

			//check click button
			if (num1 == 1)
				quit = true;
			else if (num2 == 2)
				checkScore = true;
			else if (num3 == 3)
				checkHelp = true;
			else if (num4 == 4)
			{
				quit = true;
				return false;
			}
			else if (num5 == 5)
			{
				checkScore = false;
				checkHelp = false;
			}
		}

		SDL_RenderPresent(renderer);


		if (x <= -SCREEN_WIDTH - 795)
			x = 0;
		if (count % 20 == 0)
			x--;
		count++;
		if (count % 75 == 0)
            rect++;
		if (rect > 4)
			rect = 0;
	}
	return true;
}
bool game()
{
	bool quit = false;
	bool pause = false;
	bool endGame = false;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	SDL_Event e;
	Uint32 pauseTime = 0;
	Uint32 time = 0;
	std::stringstream str_time;
	SDL_Color color = { 255, 255, 255 };

	Texture name;
	Texture yourname;
	Background bg;
	Monster mon;
	Object obj;
	Button resume;
	Button restart;
	Button title;
	Button newRestart;
	Button newTitle;
	Pause board;
	Pause ending;
	resume.loadButton(resumeButton);
	restart.loadButton(restartButton);
	title.loadButton(quitButton);
	newRestart.loadButton(restartButton);
	newTitle.loadButton(quitButton);
	board.loadBoard(resumeBoard);
	ending.loadBoard(endBoard);
	name.loadText("Enter your name", color, fontScore);

	int x = blur.get_width(), y = blur.get_height();
	int num1, num2, num3, num4, num5;
	int numX = 0, numY = -resumeBoard.get_height() + 40, nameX = 0, nameY = -endBoard.get_height() + 100, inputY = -230;
	int count = 0;
	std::string theName = "noname_";
	yourname.loadText(theName, color, font);

	while (!quit)
	{
	    bool write = false;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				restartGame = false;
				quit = true;
				return false;
			}
			else if(e.type == SDL_KEYDOWN && endGame)
            {
                if(e.key.keysym.sym == SDLK_BACKSPACE && theName.length() > 1)
                {
                    theName.pop_back();
                    theName.pop_back();
                    theName += "_";
                    write = true;
                }
            }
            else if(e.type == SDL_TEXTINPUT && theName.length() <= 30 && !keyState[SDL_SCANCODE_SPACE] && endGame)
            {
                theName.pop_back();
                theName += e.text.text;
                theName += "_";
                write = true;
            }
		}

		//dealing with time
		if (!pause && pauseTime != 0)
		{
			time = SDL_GetTicks() / 1000 - currentTime;
			currentTime += (time - pauseTime);
			pauseTime = 0;
			time = SDL_GetTicks() / 1000 - currentTime;
		}
		else if (!pause)
			time = SDL_GetTicks() / 1000 - currentTime;
		else
			pauseTime = time;
		//end with time

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); //reset screen
		SDL_RenderClear(renderer);

		if (Mix_PlayingMusic() == 0 && !endGame) //play music if it stop
			Mix_PlayMusic(music, -1);

		if (keyState[SDL_SCANCODE_ESCAPE]) //check keyboard input
			pause = true;
		if (keyState[SDL_SCANCODE_SPACE])
			pause = false;

		num1 = resume.stateButton(&e);  //get state of button
		num2 = restart.stateButton(&e);
		num3 = title.stateButton(&e);
		num4 = newRestart.stateButton(&e);
		num5 = newTitle.stateButton(&e);

		if (num1 == 6)
			pause = false;
		else if (num2 == 7 || num4 == 7)
		{
			restartGame = true;
			quit = true;
		}
		else if (num3 == 8 || num5 == 8)
		{
			restartGame = false;
			quit = true;
		}

		if (!pause && !endGame)
		{
		    char strnum[20];
		    sprintf(strnum, "%d", time);
			x = blur.get_width(), y = blur.get_height();
			mon.eventKey(count);
			text.loadText(strnum, color, font);
			bg.move(time, count);
			mon.move(count);
			obj.move(bg.get_speed(), count, mon.get_rect());
			board.moveOut();
			resume.moveOut();
			restart.moveOut();
			title.moveOut();

			numY -= 5;
			if (numY <= -240)
				numY = -240;
		}
		else if (pause && !endGame)
		{
			str_time.str("");
			str_time << "Score: " << time;
			x = 0, y = 0;
			scoreText.loadText(str_time.str().c_str(), color, fontScore);
			board.moveIn();
			resume.moveIn();
			restart.moveIn();
			title.moveIn();

			numY += 5;
			if (numY > 210)
				numY = 210;

			numX = board.get_width() / 2 + board.get_x() - scoreText.get_width() / 2;
		}
		else if(endGame)
        {
            SDL_StartTextInput();
			x = 0, y = 0;
			nameX = ending.get_width() / 2 + ending.get_x() - name.get_width() / 2;
			str_time.str("");
			str_time << "Score: " << time;
			scoreText.loadText(str_time.str().c_str(), color, fontScore);
			newRestart.moveIn();
			newTitle.moveIn();
			ending.moveIn();

			numY += 4;
			nameY += 4;
			inputY += 4;
			if (numY >= 140)
			{
				numY = 140;
				nameY = 200;
				inputY = 270;
			}
			numX = ending.get_width() / 2 + ending.get_x() - scoreText.get_width() / 2;
			Mix_HaltMusic();
        }

		if(write)
        {
            if(theName != "")
                yourname.loadText(theName, color, font);
            else
                yourname.loadText(" ", color, font);
        }

		bg.render();
		mon.render();
		obj.render();
		text.render(SCREEN_WIDTH / 2 - text.get_width() / 2, 0);
		blur.render(x, y);
		board.render(200);
		ending.render(100);
		resume.renderX(225);
		restart.renderX(350);
		title.renderX(475);
		yourname.render(SCREEN_WIDTH / 2 - yourname.get_width() / 2, inputY);
		newRestart.renderForEnd(275);
		newTitle.renderForEnd(425);
		scoreText.render(numX, numY);
		name.render(nameX, nameY);

		SDL_RenderPresent(renderer);

		if (obj.isCrash())
		{
			endGame = true;
			pause = true;
		}
		count++;
	}
	SDL_StopTextInput();
	Mix_HaltMusic();
	if(endGame)
    {
        theName.pop_back();
        if(theName.length() == 0)
            theName = "noname";
        char c[31];
        strcpy(c, theName.c_str());
        saveScore(c, time, checkScore(time));
    }
	return true;
}

bool checkOverlap(SDL_Rect a, SDL_Rect b)
{
	int xLeftA = a.x, xRightA = a.x + a.w, yTopA = a.y, yLowA = a.y + a.h;
	int xLeftB = b.x, xRightB = b.x + b.w, yTopB = b.y, yLowB = b.y + b.h;
	if (xLeftA >= xRightB)
		return false;
	else if (xRightA <= xLeftB)
		return false;
	else if (yTopA >= yLowB)
		return false;
	else if (yLowA <= yTopB)
		return false;
	return true;
}
int checkScore(int newScore)
{
	FILE* f;
	int score[5], i = 0;
	char name[5][40];
	f = fopen("score.txt", "r");
	for (i = 0; i<5; i++)
	{
		fscanf(f, "%s %d", &name[i], &score[i]);
	}
	fclose(f);
	for (i = 0; i<5; i++)
	{
		if (newScore > score[i])
			return i;
	}
	return -1;
}
void saveScore(char nameNew[31], int newScore, int pos)
{
	if (pos != -1)
	{
		FILE* f;
		int score[5], i = 0;
		char name[5][40];
		int count = 0;
		f = fopen("score.txt", "r");
		for (i = 0; i<5; i++)
		{
			fscanf(f, "%s %d", &name[i], &score[i]);
		}
		fclose(f);

		f = fopen("score.txt", "w");
		for (i = 0; i<5; i++)
		{
			if (i == pos)
			{
				fprintf(f, "%s %d\n", nameNew, newScore);
				count++;
			}
			else
				fprintf(f, "%s %d\n", name[i - count], score[i - count]);
		}
		fclose(f);
	}
}

bool gameNew()
{
    SDL_Event e;
    bool quit = false;

    int bgX = 0;

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
                quit = true;
        }
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); //reset screen
		SDL_RenderClear(renderer);

		background.render(bgX, 0);
		SDL_RenderPresent(renderer);

		if(SDL_GetTicks() % 20 == 0)
        {
            bgX -= 1;
            if(bgX <= -SCREEN_WIDTH - 800)
                bgX = 0;
        }
    }
    return true;
}

int main(int argc, char* args[])
{
	bool quit = false;
	SDL_Event e;
	init();
	loadMedia();
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}
		currentTime = SDL_GetTicks() / 1000; //current running game time
		if (!restartGame) //check game restart or not
		{
			if (!home())
				quit = true;
			else if (!game())
				quit = true;
		}
		else if (!game())
			quit = true;
	//}
	//init();
	//loadMedia();
	//gameNew();
    }
	close();
	return 0;
}

