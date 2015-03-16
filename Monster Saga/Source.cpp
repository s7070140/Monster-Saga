#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <sstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
class Character
{
	public:
		const int CHAR_WIDTH = 120;
		const int CHAR_HEIGHT = 80;

		Character();
		void eventKey(SDL_Event e);
		void move();
		void render(SDL_Rect* clip = NULL);
		void media(Texture);
	private:
		Texture texture;
		int posX;
		int posY;
		int conX;
		int conY;
};

Texture::Texture()
{
	media = NULL;
	width = 0;
	height = 0;
}
void Texture::loadFile(std::string path)
{
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
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
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

Texture nyan;

Character::Character()
{
	posX = 0;
	posY = SCREEN_HEIGHT / 2;
	conX = 0;
	conY = 0;
}
void Character::media(Texture img)
{
	texture = img;
}
void Character::eventKey(SDL_Event e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_UP: conY -= 10; break;
			case SDLK_DOWN: conY += 10; break;
			case SDLK_LEFT: conX -= 10; break;
			case SDLK_RIGHT: conX += 10; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_UP: conY += 10; break;
			case SDLK_DOWN: conY -= 10; break;
			case SDLK_LEFT: conX += 10; break;
			case SDLK_RIGHT: conX -= 10; break;
		}
	}
}
void Character::move()
{
	posX += conX;
	if (posX < 0 || posX + CHAR_WIDTH > SCREEN_WIDTH)
		posX -= conX;
	posY += conY;
	if (posY < 0 || posY + CHAR_HEIGHT > SCREEN_HEIGHT)
		posY -= conY;
}
void Character::render(SDL_Rect* clip)
{
	nyan.render(posX, posY, clip);
}

void init();
void loadMedia();
void close();
void game();
Texture background;
Texture text;
SDL_Rect sprite[6];
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
	music = Mix_LoadMUS("Sound/nyan_cat.mp3");
	font = TTF_OpenFont("Simply Rounded.ttf", 28);
	int i, num = 0;
	for (i = 0; i < 6; i++)
	{
		sprite[i] = { num, 0, 120, 80 };
		num += 120;
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
	int x = 0;
	int rect = 0;
	int x_nyan = 0, y_nyan = SCREEN_HEIGHT / 2;
	Character nyan_cat;
	std::stringstream time;
	SDL_Color color = { 255, 255, 255 };
	while (!quit)
	{
		int currentTime = SDL_GetTicks() / 1000;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
			nyan_cat.eventKey(e);
		}
		if (x == -SCREEN_WIDTH)
		{
			x = 0;
		}
		if (Mix_PlayingMusic() == 0)
			Mix_PlayMusic(music, -1);
		time.str("");
		time << "Time : " << currentTime;
		text.loadText(time.str().c_str(), color);
		background.render(x, 0, NULL);
		nyan_cat.move();
		nyan_cat.render(&sprite[rect % 6]);
		text.render(SCREEN_WIDTH - text.get_width(), 0, NULL);
		SDL_RenderPresent(renderer);
		if (x % 5 == -1)
			rect++;
		if (rect > 6)
			rect = 0;
		x--;
		SDL_Delay(10);
	}
}

int main(int argc, char* args[])
{
	init(); 
	loadMedia();
	game();
	close();
	return 0;

}