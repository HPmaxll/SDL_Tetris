#ifndef TETRIS_H_
#define TETRIS_H_

#include <SDL.h>
#include <stdbool.h>

enum GAME_STATE {
    QUIT,
    IN_MENU,
    GAME_RUN,
    GAME_PAUSE,
    GAME_OVER,
    IN_TEST
};

typedef struct {
    int x;
    int y;
} vector;

typedef struct {
    int offset_y[4];
    int offset_x[4];
} MINOS;

typedef struct {
    SDL_Renderer* renderer;
    enum GAME_STATE state;
} GAME;

extern GAME tetris;

extern const SDL_Rect tetrisWin;
extern const int tetrisWin_x, tetrisWin_y, blockSpace;
extern const MINOS minos[7][4];
extern const int refaxis[26][14];


int randint();
int drawMinos(SDL_Renderer*, vector*, MINOS*);
int drawBlock(SDL_Renderer*, int, int);
int drawRecord(SDL_Renderer*, int(*)[14]);
int drawGrid(SDL_Renderer*);
int setAxis(vector*, MINOS*, int(*)[14]);
int unsetAxis(int(*)[14]);
bool minosCollide(vector*, int(*)[14], const MINOS*);
bool checkGameOver(int(*)[14]);
int modify_x(int, int, int, int, int(*)[14], const MINOS*);
int modify_y(int, int, int, int, int(*)[14], const MINOS*);
int rotateMinos(int*, int, vector*, int(*)[14]);
int print_axis(int(*)[14]);
int getMinosY(MINOS*);
int getMinosX(MINOS*);
#endif
