#include <string>
#include <cstdint>
#include <random>
#include <vector>
#include <algorithm>
#include "sdlWrapper.hpp"
#include "basemath.hpp"

namespace SDLwrapper {
	double globalScale, requestedScale;

	std::mt19937_64 random;
	std::uniform_int_distribution<uint64_t> idGenerator(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
	
	Color::Color(uint8_t r, uint8_t g, uint8_t b) {
		Color(r, g, b, 255);
	}
	Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Font::Font(std::string filename, int fontSize) {
		fontID = idGenerator(random);
		fontResOffset = hailMath::max<double>(globalScale, globalScale);
		font = TTF_OpenFont(filename.c_str(), fontSize * fontResOffset);
	}

	Font::~Font() {
		TTF_CloseFont(font);
	}

	TTF_Font * Font::getFont() {
		return font;
	}

	SDL_Texture * Image::getImage() {
		return tex;
	}

	Image::Image(std::string filename, Window * context) {
		SDL_Surface * texLoad = IMG_Load(filename.c_str());
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // AA
		tex = SDL_CreateTextureFromSurface(context->internalGetRenderer(), texLoad);
		w = texLoad->w;
		h = texLoad->h;
		SDL_FreeSurface(texLoad);
		linkedWindow = context->windowID;
		imageID = idGenerator(random);
	}

	Image::~Image() {
		SDL_DestroyTexture(tex);
	}

	SDL_Renderer * Window::internalGetRenderer() {
		return renderer;
	}

	Window::Window(long w, long h) {
		Window(w, h, "SDL Window");
	}
	Window::Window(long w, long h, std::string name) {
		int rendererFlags, windowFlags;
		rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
		windowFlags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("Couldn't initialize SDL: %s\n", SDL_GetError());
			exit(1);
		}
		window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, windowFlags);
		if (!window) {
			printf("Failed to open %ld x %ld window: %s\n", w, h, SDL_GetError());
			exit(1);
		}
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		renderer = SDL_CreateRenderer(window, -1, rendererFlags);
		if (!renderer) {
			printf("Failed to create renderer: %s\n", SDL_GetError());
			exit(1);
		}
		resetTranslation();
		TTF_Init();
		windowID = idGenerator(random);
		int tW, tH;
		SDL_GetRendererOutputSize(renderer, &tW, &tH);
		globalScale = hailMath::min<double>((tW * 1.0) / w, (tH * 1.0) / h);
	}

	void Window::resetTranslation() {
		x = 0;
		y = 0;
		SDL_RenderSetScale(renderer, globalScale, globalScale);
	}

	void Window::translate(double x, double y) {
		this->x += x;
		this->y += y;
	}

	void Window::presentWindow() {
		SDL_RenderPresent(renderer);
	}

	void Window::scaleWindow(double s) {
		globalScale *= s;
		globalScale *= s;
		requestedScale = s;
	}

	void Window::clearScreen(Color * color) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_RenderClear(renderer);
	}

	void Window::drawRect(Color * color, double x, double y, double w, double h) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_Rect rect;
		rect.x = (x + this->x) ;
		rect.y = (y + this->y) ;
		rect.w = w ;
		rect.h = h ;
    	SDL_RenderFillRect(renderer, &rect);
	}

	void Window::drawRect(Color * color, SDL_Rect rect) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderFillRect(renderer, &rect);
	}

	void Window::strokeRect(Color * color, double x, double y, double w, double h) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
		SDL_Rect rect;
		rect.x = (x + this->x) ;
		rect.y = (y + this->y) ;
		rect.w = w ;
		rect.h = h ;
    	SDL_RenderDrawRect(renderer, &rect);
	}

	void Window::strokeRect(Color * color, SDL_Rect rect) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderDrawRect(renderer, &rect);
	}

	void Window::drawLine(Color* color, double x1, double y1, double x2, double y2) {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}

	void Window::drawImage(Image* image, double x, double y) {
		drawImage(image, x, y, image->w, image->h);
	}

	void Window::drawImage(Image* image, double x, double y, double w, double h) {
		if (image == nullptr) return;
		if (windowID != image->linkedWindow) return;
		SDL_FRect loc;
		loc.x = (x + this->x) ;
		loc.y = (y + this->y) ;
		loc.w = w ;
		loc.h = h ;
		SDL_RenderCopyF(renderer, image->getImage(), NULL, &loc);
	}

	void Window::drawImageEx(Image* image, double x, double y, bool flipH, bool flipV) {
		drawImageEx(image, x, y, image->w, image->h, flipH, flipV, 0);
	}

	void Window::drawImageEx(Image* image, double x, double y, double w, double h, bool flipH, bool flipV) {
		drawImageEx(image, x, y, image->w, image->h, flipH, flipV, 0);
	}

	void Window::drawImageEx(Image* image, double x, double y, double angle) {
		drawImageEx(image, x, y, image->w, image->h, false, false, angle);
	}

	void Window::drawImageEx(Image* image, double x, double y, double w, double h, double angle) {
		drawImageEx(image, x, y, w, h, false, false, angle);
	}

	void Window::drawImageEx(Image* image, double x, double y, bool flipH, bool flipV, double angle) {
		drawImageEx(image, x, y, image->w, image->h, flipH, flipV, angle);
	}

	void Window::drawImageEx(Image* image, double x, double y, double w, double h, bool flipH, bool flipV, double angle) {
		if (image == nullptr) return;
		if (windowID != image->linkedWindow) return;
		SDL_FRect loc;
		loc.x = (x + this->x) ;
		loc.y = (y + this->y) ;
		loc.w = w ;
		loc.h = h ;
		SDL_RendererFlip flipMode = SDL_FLIP_NONE;
		if (flipH) flipMode = (SDL_RendererFlip)(flipMode | SDL_FLIP_HORIZONTAL);
		if (flipV) flipMode = (SDL_RendererFlip)(flipMode | SDL_FLIP_VERTICAL);
		SDL_RenderCopyExF(renderer, image->getImage(), NULL, &loc, angle, NULL, flipMode);
	}

	void Window::runInput() {
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					exit(0);
					continue;
				}
				case SDL_MOUSEMOTION: {
	                SDL_GetMouseState(&mouseX, &mouseY);
	                mouseX /= globalScale;
	                mouseX /= requestedScale;
	                mouseY /= globalScale;
	                mouseY /= requestedScale;
	                break;
	            }
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if (event.key.repeat) continue;
					switch (event.key.type) {
						case SDL_KEYDOWN: {
							keysDown.push_back(event.key.keysym.sym);
							continue;
						}
						case SDL_KEYUP: {
							keysDown.erase(std::remove(keysDown.begin(), keysDown.end(), event.key.keysym.sym), keysDown.end());
							continue;
						}
						default: 
							continue;
					}
					continue;
				}
				default:
					continue;
			}
		}
	}

	void Window::quit() {
		TTF_Quit();
		SDL_Quit();
		delete this;
	}
		
	void Window::drawText(std::string str, Font * font, Color * color, double x, double y) {
		SDL_Surface * txt = TTF_RenderUTF8_Blended(font->getFont(), str.c_str(), {.r = color->r, .g = color->g, .b = color->b, .a = color->a});
		if (!txt) return;
		SDL_Texture * text = SDL_CreateTextureFromSurface(renderer, txt);
		SDL_FRect loc;
		loc.x = x + this->x;
		loc.y = y + this->y;
		loc.w = txt->w / font->fontResOffset;
		loc.h = txt->h / font->fontResOffset;
		SDL_RenderCopyF(renderer, text, NULL, &loc);
		SDL_FreeSurface(txt);
		SDL_DestroyTexture(text);
	}
		
	void Window::drawTextCentered(std::string str, Font * font, Color * color, double x, double y) {
		SDL_Surface * txt = TTF_RenderUTF8_Blended(font->getFont(), str.c_str(), {.r = color->r, .g = color->g, .b = color->b, .a = color->a});
		if (!txt) return;
		SDL_Texture * text = SDL_CreateTextureFromSurface(renderer, txt);
		SDL_FRect loc;
		loc.x = x + this->x - txt->w/font->fontResOffset/2;
		loc.y = y + this->y - txt->h/font->fontResOffset/2;
		loc.w = txt->w / font->fontResOffset;
		loc.h = txt->h / font->fontResOffset;
		SDL_RenderCopyF(renderer, text, NULL, &loc);
		SDL_FreeSurface(txt);
		SDL_DestroyTexture(text);
	}

	bool Window::keyPressed(std::string key) {
		SDL_Keycode keyCode = SDL_GetKeyFromName(key.c_str());
		return std::find(keysDown.begin(), keysDown.end(), keyCode) != keysDown.end();
	}
}