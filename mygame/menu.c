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

int initScore(char* score) {
    for (int i = 0; i < SCORE_LENGTH - 1; i++) {
        score[i] = '0';
    }
    return 0;
}

int convertScore(char* scoreString, int score) {
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

int renderScore(SDL_Renderer* renderer, char* scoreString, SDL_Texture** digitTexture, int x, int y) {
    int n;
    for (int i = 0; i < SCORE_LENGTH - 1; i++) {
        n = scoreString[i] - '0';
        SDL_RenderCopy(renderer, digitTexture[n], NULL, &(SDL_Rect) {x + i * DIGIT_WIDTH, y, DIGIT_WIDTH, DIGIT_HEIGHT});
    }
    return 0;
}
