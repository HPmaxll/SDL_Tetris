#pragma once
#ifndef MENU_H_
#define MENU_H_

#define MENU_COUNT 3
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef struct {
	int optionCount;
	char** option;
	int focus;
} menu;

SDL_Texture* loadImageFile(SDL_Renderer*, char* image);
SDL_Texture* getFontTexture(SDL_Renderer*, TTF_Font*, SDL_Color, const char*);
SDL_Texture* getCharTexture(SDL_Renderer*, TTF_Font*, SDL_Color, Uint16);

void drawRectWithBorder(SDL_Renderer*, SDL_Rect*, SDL_Color, SDL_Color);
bool withinRect(SDL_Rect*, int, int);
int initScore(char*);
int convertScore(char*, long);
int renderText(SDL_Renderer*, char*, SDL_Texture**, int, int, int, int);
#endif
