#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdlib.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
bool startGame = false;
int currentTime;

void init();
void loadMedia();
void close();
bool home();
bool game(int);
int random(int, int);
bool checkOverlap(SDL_Rect, SDL_Rect);
int newRan(int);

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

class Texture
{
public:
	Texture();
	void loadFile(std::string path);
	void free();
	void render(int, int, SDL_Rect* clip = NULL);
	void loadText(std::string text, SDL_Color textColor);
	int get_width();
	int get_height();
	int get_x();
	int get_y();
	SDL_Texture* get_texture();
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
void Texture::loadText(std::string text, SDL_Color textColor)
{
	free();
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
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

class Background
{
public:
	Background();
	void move(int);
	void stop();
	void render();
	int get_speed();
private:
	int x;
	int count;
	int speed;
	bool check;
	int current;
};
Background::Background()
{
	x = 0;
	count = 0;
	speed = 1;
	check = false;
	current = 0;
}
void Background::move(int time)
{
	if (time % 60 == 0 && !check && time != 0)
	{
		speed++;
		check = true;
		current = time;
	}
	if (time - current >= 1)
		check = false;
	if (speed > 5)
		speed = 5;
	if (x <= -SCREEN_WIDTH)
		x = 0;
	if (count % 6 == 0 && count != 0)
		x -= speed;
	count++;
}
void Background::render()
{
	background.render(x, 0, NULL);
}
int Background::get_speed()
{
	return speed;
}

Texture nyan;
SDL_Rect sprite[6];

class Monster
{
public:
	Monster();
	void eventKey();
	void move();
	void render();
	SDL_Rect get_rect();
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
	count = 0;
	current = 0;
	rect = { posX, posY, 120, 80 };
}
void Monster::eventKey()
{
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_SPACE] && count % 5 == 0)
		conY -= 2;
	else if (count % 5 == 0)
		conY += 2;
}
void Monster::move()
{
	if (count % 25 == 0)
		current++;
	posY += conY;
	if (posY <= 0 || posY + nyan.get_height() >= SCREEN_HEIGHT)
		posY -= conY;
	conY = 0;
	if (current > 6)
		current = 0;
	rect = { posX+5, posY+5, 110, 70 };
}
void Monster::render()
{
	nyan.render(posX, posY, &sprite[current % 6]);
	count++;
}
SDL_Rect Monster::get_rect()
{
	return rect;
}

Texture objectA;
SDL_Rect objSprite[4];

class Object
{
	public:
		Object();
		void move(int, SDL_Rect);
		void render();
		bool isCrash();
	private:
		int xA;
		int xB;
		int xC;
		int xD;
		int currentRectA;
		int currentRectB;
		int currentRectC;
		int currentRectD;
		int count;
		SDL_Rect a;
		SDL_Rect b;
		bool crash;
};
Object::Object()
{
	xA = SCREEN_WIDTH;
	xB = SCREEN_WIDTH;
	xC = SCREEN_WIDTH;
	xD = SCREEN_WIDTH;
	currentRectA = random(0, 3);
	currentRectB = random(0, 3);
	currentRectC = random(0, 3);
	currentRectD = random(0, 3);
	count = 0;
	a = { xA, 0, objSprite[currentRectA].w, objSprite[currentRectA].h };
	b = { xC, SCREEN_HEIGHT - objSprite[currentRectC].h, objSprite[currentRectC].w, objSprite[currentRectC].h };
	crash = false;
}
void Object::move(int speed, SDL_Rect monster)
{
	if (xA + objSprite[currentRectA].w <= 0)
		currentRectA = random(0, 3);
	if (currentRectB == NULL)
		currentRectB = random(0, 3);
	if (currentRectA == currentRectC)
		currentRectC = random(0, 3);
	if (xC + objSprite[currentRectC].w <= 0)
		currentRectC = currentRectD;
	if (currentRectB == currentRectD)
		currentRectD = random(0, 3);
	if (currentRectD == NULL)
		currentRectD = random(0, 3);
	if (xA + objSprite[currentRectA].w <= 0)
	{
		xA = xB;
		currentRectA = currentRectB;
		xB = SCREEN_WIDTH;
		currentRectB = NULL;

		xC = xD;
		currentRectC = currentRectD;
		xD = SCREEN_WIDTH;
		currentRectD = NULL;
	}

	if (count % 6 == 0)
	{
		xA -= speed;
		xC -= speed;
	}
	if (xA <= SCREEN_WIDTH / 2 && count % 6 == 0)
	{
		xB -= speed;
		xD -= speed;
	}
	count++;
	a = { xA, 0, objSprite[currentRectA].w, objSprite[currentRectA].h };
	b = { xC, SCREEN_HEIGHT - objSprite[currentRectC].h, objSprite[currentRectC].w, objSprite[currentRectC].h };
	
	if (checkOverlap(monster, a))
		crash = true;
	if (checkOverlap(monster, b))
		crash = true;
}
void Object::render()
{
	objectA.render(xA, 0, &objSprite[currentRectA]);
	objectA.render(xB, 0, &objSprite[currentRectB]);
	objectA.render(xC, SCREEN_HEIGHT - objSprite[currentRectC].h, &objSprite[currentRectC]);
	objectA.render(xD, SCREEN_HEIGHT - objSprite[currentRectD].h, &objSprite[currentRectD]);
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
SDL_Rect buttonClip[3];
SDL_Rect closeClip[3];
SDL_Rect pressRect[5];
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
	private:
		Texture button;
		int left, right;
		int top, low;
		int appear;
		int state;
		int closeB;
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
				if (!press_sound)
				{
					Mix_PlayChannel(-1, mousePress, 0);
					press_sound = true;
				}
				state = 2;
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				state = 1;
				if (top == 150)
					return 1;
				else if (top == 250)
					return 2;
				else if (top == 350)
					return 3;
				else if (top == 450)
					return 4;
				else if (low == button.get_y() + closeButton.get_height() / 3)
					return 5;
				break;
			}
		}
	}
}
void Button::moveIn()
{
	appear += 5;
	closeB += 5;
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
}
void Button::moveOut()
{
	appear -= 5;
	closeB -= 5;
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
	background.loadFile("Image Stuff/background.png");
	nyan.loadFile("Image Stuff/nyan-cat.png");
	objectA.loadFile("Image Stuff/wall.png");
	press_text.loadFile("Image Stuff/press_text.png");
	music = Mix_LoadMUS("Sound/nyan_cat.mp3");
	font = TTF_OpenFont("Simply Rounded.ttf", 28);
	logo.loadFile("Image Stuff/logo.png");
	menuText.loadFile("Image Stuff/menu.png");
	startButton.loadFile("Image Stuff/startButton.png");
	scoreButton.loadFile("Image Stuff/scoreButton.png");
	helpButton.loadFile("Image Stuff/helpButton.png");
	exitButton.loadFile("Image Stuff/exitButton.png");
	bgBoard.loadFile("Image Stuff/bgBoard.png");
	closeButton.loadFile("Image Stuff/closeButton.png");
	bgBoard.loadFile("Image Stuff/bgBoard.png");
	helpBoard.loadFile("Image Stuff/helpBoard.png");
	mouseIn = Mix_LoadWAV("Sound/01_inside.wav");
	mousePress = Mix_LoadWAV("Sound/02_press.wav");

	int i, num = 0;
	for (i = 0; i < 6; i++)
	{
		sprite[i] = { num, 0, 120, 80 };
		num += 120;
	}
	objSprite[0] = { 0, 0, 100, 350 };
	objSprite[1] = { 200, 0, 100, 250 };
	objSprite[2] = { 400, 0, 100, 200 };
	objSprite[3] = { 600, 0, 100, 100 };

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
}
void close()
{//close and clear all
	background.free();
	nyan.free();
	text.free();
	objectA.free();
	logo.free();
	menuText.free();
	startButton.free();
	scoreButton.free();
	helpButton.free();
	exitButton.free();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	Mix_FreeChunk(mouseIn);
	Mix_FreeChunk(mousePress);
	font = NULL;
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

	int count = 0, x = 0, rect = 0;
	int time;
	bool checkScore = false;
	bool checkHelp = false;

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
			}
			menu.renderMenu();
			start.render(100);
			score.render(200);
			help.render(300);
			exit.render(400);
			highScore.render();
			howToPlay.render();
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


		if (x <= -SCREEN_WIDTH)
			x = 0;
		if (count % 10 == 0)
			x--;
		count++;
		if (count % 75 == 0)
			rect++;
		if (rect > 4)
			rect = 0;
	}
	return true;
}
bool game(int oldTime)
{
	SDL_Event e;
	bool quit = false;
	Background bg;
	Monster mon;
	Object obj;
	SDL_Color color = { 255, 255, 255 };

	while (!quit)
	{
		Uint32 time = SDL_GetTicks() / 1000 - oldTime;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return false;
			}
		}
		mon.eventKey();
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); //reset screen
		SDL_RenderClear(renderer);

		if (Mix_PlayingMusic() == 0) //play music if it stop
			Mix_PlayMusic(music, -1);

		text.loadText(std::to_string(time), color);
		bg.move(time);
		mon.move();
		obj.move(bg.get_speed(), mon.get_rect());

		bg.render();
		mon.render();
		obj.render();
		text.render(SCREEN_WIDTH - text.get_width(), 0, NULL);
		SDL_RenderPresent(renderer);

		if (obj.isCrash())
		{
			quit = true;
			Mix_HaltMusic();
		}
	}
	return true;
}

int random(int min, int max)
{
	return rand() % (max - min + 1) + min;
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

int main(int argc, char* args[])
{
	bool quit = false;
	init();
	loadMedia();
	while (!quit)
	{
		if (!home())
			quit = true;
		else if (!game(currentTime))
			quit = true;
	}
	close();
	return 0;
}