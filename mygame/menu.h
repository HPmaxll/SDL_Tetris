#pragma once
#ifndef MENU_H_
#define MENU_H_

#define MENU_COUNT 3
#include <SDL.h>
#include <SDL_ttf.h>
#include "constants.h"

extern SDL_Color menuFg;
extern SDL_Color menuBg;
extern SDL_Rect menuWin;
extern SDL_Rect popupWin;
extern SDL_Rect fontWin;

extern SDL_Rect optionWin[3];

extern char* menuOption[MENU_COUNT];
extern char* menuSelect[MENU_COUNT];

SDL_Texture* getFontTexture(SDL_Renderer*, TTF_Font*, SDL_Color, const char*);
SDL_Texture* getCharTexture(SDL_Renderer*, TTF_Font*, SDL_Color, Uint16);
int initScore(char*);
int convertScore(char*, int);
int renderScore(SDL_Renderer*, char*, SDL_Texture**, int x, int y);
#endif
