#pragma once
#ifndef UI_H_
#define UI_H_

#define MENU_COUNT 3
#define setDrawColor(x, y) SDL_SetRenderDrawColor(x, y.r, y.g, y.b, y.a)

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef struct {
	int optionCount;
	char** option;
	int focus;
} menu;

typedef struct {
	SDL_Color main_bg;
	SDL_Color main_fg;
	SDL_Color menu_bg;
	SDL_Color menu_fg;
	SDL_Color game_bg;
	SDL_Color grid;
	SDL_Color border;
} SCHEME;

extern const SCHEME scheme[2];
SDL_Texture* loadImageFile(SDL_Renderer*, char* image);
SDL_Texture* getFontTexture(SDL_Renderer*, TTF_Font*, SDL_Color, const char*);
SDL_Texture* getCharTexture(SDL_Renderer*, TTF_Font*, SDL_Color, Uint16);
void drawRectWithBorder(SDL_Renderer*, SDL_Rect*, SDL_Color, SDL_Color);
bool withinRect(SDL_Rect*, int, int);
int initScore(char*);
int convertScore(char*, long);
int renderText(SDL_Renderer*, char*, SDL_Texture**, int, int, int, int);
#endif
