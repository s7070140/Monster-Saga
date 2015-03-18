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

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font;

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
private:
	int posX;
	int posY;
	int conY;
	int count;
	int current;
};
Monster::Monster()
{
	posX = (SCREEN_WIDTH / 2) / 2;
	posY = SCREEN_HEIGHT / 2;
	conY = 0;
	count = 0;
	current = 0;
}
void Monster::eventKey()
{
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_SPACE] && count % 5 == 0)
		conY -= 2;
	else if (count % 10 == 0)
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
}
void Monster::render()
{
	nyan.render(posX, posY, &sprite[current % 6]);
	count++;
}

Texture text;
Texture object;
SDL_Rect objSprite[10];
Mix_Music *music;

void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Monster Saga", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	TTF_Init();
}
void loadMedia()
{
	background.loadFile("Image Stuff/background.png");
	nyan.loadFile("Image Stuff/nyan-cat.png");
	//object.loadFile("Image Stuff/object.png");
	music = Mix_LoadMUS("Sound/nyan_cat.mp3");
	font = TTF_OpenFont("Simply Rounded.ttf", 28);
	int i, num = 0, height = 75;
	for (i = 0; i < 6; i++)
	{
		sprite[i] = { num, 0, 120, 80 };
		num += 120;
	}
	num = 0;
	for (i = 0; i < 8; i++)
	{
		objSprite[i] = { num, 0, 100, height };
		height += 75;
		num += 100;
	}
}
void close()
{
	background.free();
	nyan.free();
	text.free();
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
	SDL_Color color = { 255, 255, 255 };
	int rect = 0;

	while (!quit)
	{
		Uint32 time = SDL_GetTicks() / 1000;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}
		mon.eventKey();
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		if (Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music, -1);

		text.loadText(std::to_string(time), color);
		bg.move(time);
		mon.move();

		bg.render();
		mon.render();
		text.render(SCREEN_WIDTH - text.get_width(), 0, NULL);
		SDL_RenderPresent(renderer);
	}
}

int random(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

int main(int argc, char* args[])
{
	init();
	loadMedia();
	game();
	close();
	return 0;

}