#include <stdio.h>
#include <string.h>
#include "tetris.h"
#include "menu.h"
#include "constants.h"

int FPS = 60;
int fallingTime = 500;
int level = 1;
int menuFocus = 0;
int optFocus = 0;
int menuID = 0;
int score = 0;
int bg_index = 0;
int gridState = 0;

char scoreString[SCORE_LENGTH] = "0000000000\0";

char* mainMenu[] = {
    "Start",
    "Option",
    "Quit"
};

char* optMenu[] = {
    "Background Color:",
    "Level:",
    "Grid:"
};

char* switchStr[] = {
    "On",
    "Off"
};

SDL_Color menuFg = { 0, 0, 100, 255 };
SDL_Rect menuWin = { (WIN_WIDTH - 361) / 2, 80, 361, 128 };
SDL_Rect popupWin = { (WIN_WIDTH - 240) / 2, (WIN_HEIGHT - 64) / 2, 240, 64 };

int main(int argc, char* argv[]) {

    Uint32 timeStart, timeEnd, curTime, prevTime;
    int frameTime = 1000 / FPS;
    
    int minosType, minosAngle, nextType, nextAngle, removeCount;
    int refpos[26][14];
    MINOS curMinos, nextMinos;
    vector tetrisCord, tmpCord, displayCord;
    
    SDL_Window * window;
    GAME tetris;
    SDL_Texture * ASCII[95];
    SDL_Texture * TitleTexture, * overTexture, * pauseTexture, * focusLeft, * focusRight;
    TTF_Font * TitleFont, * MainOptionFont;
    SDL_Rect popupBack = { 0, (WIN_HEIGHT - 80) / 2, WIN_WIDTH, 70 };

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    window = SDL_CreateWindow(
        "Tetris",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    tetris.renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
  
    TitleFont = TTF_OpenFont("assets/fonts/BasculaCollege.ttf", 128);
    MainOptionFont = TTF_OpenFont("assets/fonts/NotoSansMonoCJKsc-Regular.otf", 20);

    TitleTexture = getFontTexture(tetris.renderer, TitleFont, menuFg, "Tetris");
    for (int i = 0; i < 95; i++) {
        ASCII[i] = getCharTexture(tetris.renderer, MainOptionFont, menuFg, i + 32);
    }
    overTexture = getFontTexture(tetris.renderer, TitleFont, (SDL_Color) {0, 0, 0, 255}, "GAME OVER");
    pauseTexture = getFontTexture(tetris.renderer, TitleFont, (SDL_Color) { 125, 125, 125, 255 }, "  PAUSE  ");

    TTF_CloseFont(TitleFont);
    TTF_CloseFont(MainOptionFont);

    focusLeft = loadImageFile(tetris.renderer, "assets/pics/left.png");
    focusRight = loadImageFile(tetris.renderer, "assets/pics/right.png");

    tetris.state = IN_MENU;
    /* Game Loop */
    while (tetris.state != QUIT) {

        timeStart = SDL_GetTicks();

        SDL_Event event;
        switch (tetris.state) {
        case IN_MENU:
            switch (menuID) {
            case 0:
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.scancode) {

                        case SDL_SCANCODE_UP:
                        case SDL_SCANCODE_W:
                            menuFocus -= 1;
                            if (menuFocus < 0)
                                menuFocus = 2;
                            break;

                        case SDL_SCANCODE_DOWN:
                        case SDL_SCANCODE_S:
                            menuFocus += 1;
                            if (menuFocus > 2)
                                menuFocus = 0;
                            break;

                        case SDL_SCANCODE_RETURN:
                            switch (menuFocus) {
                            case 0:
                                tetris.state = GAME_RUN;
                                SDL_memcpy(refpos, refaxis, sizeof(refaxis));
                                prevTime = SDL_GetTicks();
                                minosType = randint() % 7;
                                minosAngle = randint() % 4;
                                curMinos = minos[minosType][minosAngle];
                                tetrisCord.x = getMinosX(&curMinos);
                                tetrisCord.y = getMinosY(&curMinos);

                                nextType = (randint() + 1) % 7;
                                nextAngle = (randint() + 1) % 4;
                                nextMinos = minos[nextType][nextAngle];
                                displayCord.x = 8 + getMinosX(&nextMinos);
                                displayCord.y = 6 + getMinosY(&nextMinos);
                                break;
                            case 1:
                                menuID = 1;
                                break;
                            case 2:
                                tetris.state = QUIT;
                                break;
                            }
                            break;
                        }
                        break;
                    case SDL_QUIT:
                        tetris.state = QUIT;
                        break;
                    }
                }

                SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(tetris.renderer);
                SDL_RenderCopy(tetris.renderer, TitleTexture, NULL, &menuWin);
                for (int i = 0; i < MENU_COUNT; i++) {
                    size_t optLen = strlen(mainMenu[i]);
                    renderText(tetris.renderer, mainMenu[i], ASCII, (WIN_WIDTH - optLen * CHAR_WIDTH) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }
                SDL_RenderCopy(tetris.renderer, focusLeft, NULL, &(SDL_Rect) {210, 40 * menuFocus + 245, 20, 20});
                SDL_RenderCopy(tetris.renderer, focusRight, NULL, &(SDL_Rect) {WIN_WIDTH - 230, 40 * menuFocus + 245, 20, 20});
                break;

            case 1:
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                    
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            switch (optFocus) {
                            case 0:
                                if (bg_index > 0)
                                    bg_index -= 1;
                                break;
                            case 1:
                                if (level > 1)
                                    level -= 1;
                                break;
                            case 2:
                                if (gridState == 1)
                                    gridState = 0;
                                else
                                    gridState = 1;
                                break;
                            }
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            switch (optFocus) {
                            case 0:
                                if (bg_index < 8)
                                    bg_index += 1;
                                break;
                            case 1:
                                if (level < 5)
                                    level += 1;
                                break;
                            case 2:
                                if (gridState == 0)
                                    gridState = 1;
                                else
                                    gridState = 0;
                                break;
                            }
                            break;
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            optFocus -= 1;
                            if (optFocus < 0)
                                optFocus = 2;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            optFocus += 1;
                            if (optFocus > 2)
                                optFocus = 0;
                            break;
                        case SDL_SCANCODE_RETURN:
                            menuID = 0;
                            break;
                        }
                        break;
                    case SDL_QUIT:
                        tetris.state = QUIT;
                        break;
                    }
                }
                SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(tetris.renderer);
                SDL_RenderCopy(tetris.renderer, TitleTexture, NULL, &menuWin);
                
                SDL_SetRenderDrawColor(tetris.renderer, 160, 160, 160, 255);
                SDL_RenderFillRect(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2 + 5, 225, 320, 200});
                
                drawRectWithBorder(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2, 220, 320, 200}, tetris_color[8], tetris_color[7]);
                
                for (int i = 0; i < MENU_COUNT; i++) {
                    size_t optLen = strlen(optMenu[i]);
                    renderText(tetris.renderer, optMenu[i], ASCII, (WIN_WIDTH - 300) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }

                drawRectWithBorder(tetris.renderer, &(SDL_Rect){355, 245, 25, 25}, tetris_color[bg_index], tetris_color[7]);
                renderText(tetris.renderer, (char[2]) { level + '0', '\0' }, ASCII, 360, 280, CHAR_WIDTH, CHAR_HEIGHT);
                renderText(tetris.renderer, switchStr[gridState], ASCII, 360, 320, CHAR_WIDTH, CHAR_HEIGHT);

                SDL_RenderCopy(tetris.renderer, focusLeft, NULL, &(SDL_Rect) {325, 40 * optFocus + 245, 20, 20});
                SDL_RenderCopy(tetris.renderer, focusRight, NULL, &(SDL_Rect) {390, 40 * optFocus + 245, 20, 20});
                break;
            }
            break;

        case GAME_RUN:

            /* Handle Event */
            while (SDL_PollEvent(&event)) {

                switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        rotateMinos(&minosAngle, minosType, &tetrisCord, refpos);
                        curMinos = minos[minosType][minosAngle];
                        break;

                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        tmpCord.x = tetrisCord.x;
                        tmpCord.y = tetrisCord.y + 1;
                        
                        if (!minosCollide(&tmpCord, refpos, &curMinos)) {
                            tetrisCord.y = tmpCord.y;
                            prevTime = SDL_GetTicks();
                        }
                        
                        break;

                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        tetrisCord.x -= 1;
                        if (minosCollide(&tetrisCord, refpos, &curMinos))
                            tetrisCord.x += 1;
                        break;

                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        tetrisCord.x += 1;
                        if (minosCollide(&tetrisCord, refpos, &curMinos))
                            tetrisCord.x -= 1;
                        break;

                    case SDL_SCANCODE_RETURN:
                        tetris.state = GAME_PAUSE;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            /* Game Logic */
            curTime = SDL_GetTicks();
            if (curTime - prevTime >= fallingTime) {
                tetrisCord.y += 1;
                prevTime = curTime;
            }
            if (minosCollide(&tetrisCord, refpos, &curMinos)) {
                tetrisCord.y -= 1;
                setAxis(&tetrisCord, &curMinos, refpos);
                removeCount = unsetAxis(refpos);

                score += removeCount * removeCount * 100;
                convertScore(scoreString, score);

                minosType = nextType;
                minosAngle = nextAngle;
                curMinos = minos[minosType][minosAngle];
                tetrisCord.x = getMinosX(&curMinos);
                tetrisCord.y = getMinosY(&curMinos);

                nextType = randint() % 7;
                nextAngle = randint() % 4;
                nextMinos = minos[nextType][nextAngle];
                displayCord.x = 8 + getMinosX(&nextMinos);
                displayCord.y = 6 + getMinosY(&nextMinos);

            }
            if (checkGameOver(refpos))
                tetris.state = GAME_OVER;

            SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(tetris.renderer);
            renderText(tetris.renderer, scoreString, ASCII, tetrisWin_x, 10, 10, 30);
            drawGrid(tetris.renderer);
            drawRecord(tetris.renderer, refpos);
            drawMinos(tetris.renderer, &displayCord, &nextMinos);
            drawMinos(tetris.renderer, &tetrisCord, &curMinos);
            
            break;

        case GAME_PAUSE:
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_W:
                        break;
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        break;
                    case SDL_SCANCODE_RETURN:
                        tetris.state = GAME_RUN;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(tetris.renderer);
            drawGrid(tetris.renderer);
            drawRecord(tetris.renderer, refpos);
            SDL_SetRenderDrawColor(tetris.renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(tetris.renderer, &popupBack);
            SDL_RenderCopy(tetris.renderer, pauseTexture, NULL, &popupWin);
            break;

        case GAME_OVER:
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_W:
                        break;
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        break;
                    case SDL_SCANCODE_RETURN:
                        tetris.state = QUIT;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(tetris.renderer);
            drawGrid(tetris.renderer);
            drawRecord(tetris.renderer, refpos);
            SDL_SetRenderDrawColor(tetris.renderer, 255, 100, 100, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(tetris.renderer, &popupBack);
            SDL_RenderCopy(tetris.renderer, overTexture, NULL, &popupWin);
            break;

        case IN_TEST:
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_W:
                        break;
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        break;
                    case SDL_SCANCODE_RETURN:
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, 255);
            SDL_RenderClear(tetris.renderer);
            SDL_SetRenderDrawColor(tetris.renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 200)/2, (WIN_HEIGHT - 200)/2, 200, 200});
            SDL_SetRenderDrawColor(tetris.renderer, 200, 200, 100, 255);
            SDL_RenderFillRect(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 200) / 2 + 1, (WIN_HEIGHT - 200) / 2 + 1 , 198, 198});
      
            break;
        }

        /* Limit Frame */
        timeEnd = SDL_GetTicks();
        if ((timeEnd - timeStart) < frameTime)
            SDL_Delay(frameTime - (timeEnd - timeStart));

        /* Present Renderer */
        SDL_RenderPresent(tetris.renderer);
    }

    SDL_DestroyRenderer(tetris.renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}