#include "constants.h"
#include "menu.h"
#include <windows.h>
#include <stdio.h>

SDL_Texture* loadImageFile(SDL_Renderer* renderer, char* image) {
    SDL_Surface* surf = IMG_Load(image);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return Texture;
}
SDL_Texture* getFontTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const char* text) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);
    //printf("%d, %d\n", surf->w, surf->h);
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

void drawRectWithBorder(SDL_Renderer* renderer, SDL_Rect* rect, SDL_Color fill, SDL_Color border) {
    SDL_Rect tmp = { rect->x + 1, rect->y + 1, rect->w - 2, rect->h - 2 };
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &tmp);
}

bool withinRect(SDL_Rect* rect, int x, int y) {
    if (x >= rect->x && y >= rect->y && x <= rect->x + rect->w && y <= rect->y + rect->h)
        return true;
    else
        return false;
}

int initScore(char* scoreString) {
    for (int i = 7; i < SCORE_LENGTH - 1; i++) {
        scoreString[i] = '0';
    }
    return 0;
}

int convertScore(char* scoreString, long score) {
    int i = 0;
    int tmp;
    do {
        tmp = score % 10;
        score /= 10;
     
        scoreString[SCORE_LENGTH - 2 - i] = tmp + '0';
        i++;
    } while (score != 0);
    
    return 0;
}

int renderText(SDL_Renderer* renderer, char* string, SDL_Texture** ASCII, int x, int y, int char_width, int char_height) {
    int n;
    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        n = string[i] - 32;
        SDL_RenderCopy(renderer, ASCII[n], NULL, &(SDL_Rect) {x + i * char_width, y, char_width, char_height});
    }
    return 0;
}