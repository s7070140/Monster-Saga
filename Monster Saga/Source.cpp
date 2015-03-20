#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdlib.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void init();
void loadMedia();
void close();
void game();
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
	void render(int, int, SDL_Rect*);
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
Mix_Music* music;

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
	music = Mix_LoadMUS("Sound/nyan_cat.mp3");
	font = TTF_OpenFont("Simply Rounded.ttf", 28);
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
}
void close()
{//close and clear all
	background.free();
	nyan.free();
	text.free();
	objectA.free();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	font = NULL;
	window = NULL;
	renderer = NULL;
	SDL_Quit();
	IMG_Quit();
	Mix_Quit();
	TTF_Quit();
}
void game()
{
	SDL_Event e;
	bool quit = false;
	Background bg;
	Monster mon;
	Object obj;
	SDL_Color color = { 255, 255, 255 };

	while (!quit)
	{
		Uint32 time = SDL_GetTicks() / 1000;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
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
			quit = true;
	}
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
	init();
	loadMedia();
	game();
	close();
	return 0;

}