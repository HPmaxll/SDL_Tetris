#include "tetris.h"
#include "constants.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const SDL_Rect tetrisWin = { (WIN_WIDTH - TETRIS_WIDTH) / 2, (WIN_HEIGHT - TETRIS_HEIGHT) / 2, TETRIS_WIDTH + 1, TETRIS_HEIGHT + 1 };
const int tetrisWin_x = (WIN_WIDTH - TETRIS_WIDTH) / 2;
const int tetrisWin_y = (WIN_HEIGHT - TETRIS_HEIGHT) / 2;
const int blockSpace = (BLOCK_WIDTH + BLOCK_GAP);

const SDL_Color tetris_color[] = {
    {255, 0, 0, 255},
    {255, 165, 0, 255},
    {255, 255, 0, 255},
    {0, 255, 0, 255},
    {0, 255, 255, 255},
    {0, 0, 255, 255},
    {139, 0, 255, 255}
};

const MINOS minos[7][4] = {
    {
        { {0,0,0,0},{-2,-1,0,1},1 },
        { {-1,0,1,2},{0,0,0,0},1 },
        { {0,0,0,0},{-1,0,1,2},1 },
        { {-2,-1,0,1},{0,0,0,0},1 }},
    {
        { {0,0,1,1},{0,1,0,1},2 },
        { {0,0,1,1},{0,1,0,1},2 },
        { {0,0,1,1},{0,1,0,1},2 },
        { {0,0,1,1},{0,1,0,1},2 }},
    {
        { {-1,-1,0,0},{-1,0,0,1},3 },
        { {-1,0,0,1},{0,-1,0,-1},3 },
        { {0,0,1,1},{-1,0,0,1},3 },
        { {-1,0,0,1},{1,0,1,0},3 }},
    {
        { {-1,-1,0,0},{0,1,-1,0},4 },
        { {-1,0,0,1},{-1,-1,0,0},4 },
        { {0,0,1,1},{0,1,-1,0},4 },
        { {-1,0,0,1},{0,0,1,1},4 }},
    {
        { {-1,0,0,0},{-1,-1,0,1},5 },
        { {-1,0,1,1},{0,0,-1,0},5 },
        { {0,0,0,1},{-1,0,1,1},5 },
        { {-1,-1,0,1},{0,1,0,0},5 }},
    {
        { {-1,0,0,0},{0,-1,0,1},6 },
        { {-1,0,0,1},{0,-1,0,0},6 },
        { {0,0,0,1},{-1,0,1,0},6 },
        { {-1,0,0,1},{0,0,1,0},6 }},
    {
        { {-1,0,0,0},{1,-1,0,1},7 },
        { {-1,-1,0,1},{-1,0,0,0},7 },
        { {0,0,0,1},{-1,0,1,-1},7 },
        { {-1,0,1,1},{0,0,0,1},7 }}
};

const int refaxis[26][14] = {
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int randint(void)
{
    srand(time(NULL));
    return rand();
}

int drawMinos(SDL_Renderer* renderer, vector* point, MINOS* minos) {
    SDL_Color tmpColor = tetris_color[minos->color - 1];
    SDL_SetRenderDrawColor(renderer, tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
    for (int i = 0; i < 4; i++) {
        if (minos->offset_y[i] + point->y >= 4)
            drawBlock(renderer, minos->offset_x[i] + point->x, minos->offset_y[i] + point->y - 4);
    }
    return 0;
}

int drawBlock(SDL_Renderer* renderer, int x, int y) {
    SDL_Rect block = {
                tetrisWin_x + x * blockSpace + (BLOCK_GAP + 1) / 2,
                tetrisWin_y + y * blockSpace + (BLOCK_GAP + 1) / 2,
                BLOCK_WIDTH,
                BLOCK_WIDTH
    };
    SDL_RenderFillRect(renderer, &block);
    return 0;
}

int drawRecord(SDL_Renderer* renderer, int(*pos)[14]) {
    for (int i = 2; i < 12; i++) {
        for (int j = 4; j < 24; j++) {
            if (pos[j][i] != 0) {
                SDL_Color tmpColor = tetris_color[pos[j][i] - 1];
                SDL_SetRenderDrawColor(renderer, tmpColor.r, tmpColor.g, tmpColor.b, tmpColor.a);
                drawBlock(renderer, i - 2, j - 4);
            }
        }
    }
    return 0;
}

int drawGrid(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 1; i < 10; i++)
        SDL_RenderDrawLine(renderer, tetrisWin_x + i * blockSpace, tetrisWin_y + 1, tetrisWin_x + i * blockSpace, tetrisWin_y + TETRIS_HEIGHT - 2);
    for (int i = 1; i < 20; i++)
        SDL_RenderDrawLine(renderer, tetrisWin_x + 1, tetrisWin_y + i * blockSpace, tetrisWin_x + TETRIS_WIDTH - 2, tetrisWin_y + i * blockSpace);
    return 0;
}

bool minosCollide(vector* point, int(*pos)[14], const MINOS* shape) {
    int pos_y, pos_x;
    bool collision = false;
    for (int i = 0; i < 4; i++) {
        pos_y = point->y + shape->offset_y[i];
        pos_x = point->x + 2 + shape->offset_x[i];
        if (pos[pos_y][pos_x] != 0) {
            collision = true;
            break;
        }
    }
    return collision;
}

int setAxis(vector* point, MINOS* minos, int(*pos)[14]) {
    for (int i = 0; i < 4; i++) {
        pos[minos->offset_y[i] + point->y][minos->offset_x[i] + point->x + 2] = minos->color;
    }
    return 0;
}

int unsetAxis(int(*pos)[14]) {
    int count = 0;
    for (int i = 4; i < 24; i++) {
        for (int j = 2; j < 12; j++) {
            if (pos[i][j] == 0)
                break;
            if (j == 11) {
                count += 1;
                for (int m = i; m >= 4; m--) {
                    for (int n = 2; n < 12; n++) {
                        if (m == 0)
                            pos[m][n] = 0;
                        else
                            pos[m][n] = pos[m - 1][n];
                    }
                }
            }
        }
    }
    return count;
}

bool checkGameOver(int(*pos)[14]) {
    for (int i = 2; i < 12; i++) {
        for (int j = 3; j >= 0; j--) {
            if (pos[j][i] != 0)
                return true;
        }
    }
    return false;
}

int modify_x(int prev_left, int prev_right, int y, int x, int(*pos)[14], const MINOS* shape) {
    int i, pos_x, pos_y, left, right;
    left = right = 0;
    for (i = 0; i < 4; i++) {
        pos_y = y + shape->offset_y[i];
        pos_x = x + 2 + shape->offset_x[i];
        if (pos_x < 1) {
            left = 1;
            right = 0;
            break;
        }
        if (pos_x > 11) {
            left = 0;
            right = 1;
            break;
        }
        if (pos[pos_y][pos_x] != 0) {
            if (pos_x < x + 2)
                left = 1;
            if (pos_x > x + 2)
                right = 1;
            if (pos_x == x + 2)
                return -1;
        }
    }
    if (left == 0 && right == 0)
        return x;
    if ((left == 1 && right == 1) || (left == 1 && prev_right == 1) || (prev_left == 1 && right == 1))
        return -1;
    if (left == 1)
        return modify_x(1, 0, y, x + 1, pos, shape);
    if (right == 1)
        return modify_x(0, 1, y, x - 1, pos, shape);
}

int modify_y(int prev_up, int prev_down, int y, int x, int(*pos)[14], const MINOS* shape) {
    int i, pos_x, pos_y, up, down;
    up = down = 0;
    for (i = 0; i < 4; i++) {
        pos_y = y + shape->offset_y[i];
        pos_x = x + 2 + shape->offset_x[i];
        if (pos[pos_y][pos_x] != 0) {
            if (pos_y < y) {
                if (pos[pos_y + 1][pos_x] == 1)
                    return -1;
                else
                    up = 1;
            }
            if (pos_y > y) {
                if (pos[pos_y - 1][pos_x] == 1)
                    return -1;
                else
                    down = 1;
            }
            if (pos_y == y)
                return -1;
        }
    }
    if (up == 0 && down == 0)
        return y;
    if ((up == 1 && down == 1) || (up == 1 && prev_down == 1) || (prev_up == 1 && down == 1))
        return -1;
    if (up == 1)
        return modify_y(1, 0, y + 1, x, pos, shape);
    if (down == 1)
        return modify_y(0, 1, y - 1, x, pos, shape);
}

int rotateMinos(int* minosAngle, int minosType, vector* point, int(*pos)[14]) {
    bool rotate = false;
    int tmp_x, tmp_y;
    int minosAngleTmp = *minosAngle;
    minosAngleTmp += 1;
    if (minosAngleTmp == 4)
        minosAngleTmp = 0;
    tmp_y = modify_y(0, 0, point->y, point->x, pos, &minos[minosType][minosAngleTmp]);
    if (tmp_y != -1) {
        tmp_x = modify_x(0, 0, tmp_y, point->x, pos, &minos[minosType][minosAngleTmp]);
        if (tmp_x != -1)
            rotate = true;
    }
    else {
        tmp_x = modify_x(0, 0, point->y, point->x, pos, &minos[minosType][minosAngleTmp]);
        if (tmp_x != -1) {
            tmp_y = modify_y(0, 0, point->y, tmp_x, pos, &minos[minosType][minosAngleTmp]);
            if (tmp_y != -1)
                rotate = true;
        }
    }
    if (rotate) {
        point->x = tmp_x;
        point->y = tmp_y;
        *minosAngle = minosAngleTmp;
    }
    return 0;
}

int getMinosY(MINOS* minos) {
    int max = 0;
    for (int i = 0; i < 4; i++) {
        if (max < minos->offset_y[i])
            max = minos->offset_y[i];
    }
    return 3 - max;
}
int getMinosX(MINOS* minos) {
    int min = 0;
    int max = 0;
    for (int i = 0; i < 4; i++) {
        if (min > minos->offset_x[i])
            min = minos->offset_x[i];
        if (max < minos->offset_x[i])
            max = minos->offset_x[i];
    }
    return (10 - max - min) / 2;
}