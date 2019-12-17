#include "constants.h"
#include "menu.h"
#include <windows.h>

char* menuOption[MENU_COUNT] = {
    "Start",
    "Option",
    "Quit"
};

char* menuSelect[MENU_COUNT] = {
    "+ Start +",
    "+ Option +",
    "+ Quit +"
};

SDL_Color menuFg = { 0, 0, 100, 255 };
SDL_Color menuBg = { 255, 255, 255, 255 };
SDL_Rect menuWin = { (WIN_WIDTH - 300) / 2, 80, 300, 128 };
SDL_Rect popupWin = { (WIN_WIDTH - 240) / 2, (WIN_HEIGHT - 64) / 2, 240, 64 };

SDL_Rect swordWin = { (WIN_WIDTH - 256) / 2, 100, 256, 256 };
SDL_Rect fontWin = { 100, 100, 12, 24 };

SDL_Texture* getFontTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const char* text) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return Texture;
}

SDL_Texture* getCharTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Uint16 ch) {
    SDL_Surface* surf = TTF_RenderGlyph_Blended(font, ch, color);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return Texture;
}
