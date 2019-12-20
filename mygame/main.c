#include <stdio.h>
#include <string.h>
#include "tetris.h"
#include "menu.h"
#include "constants.h"

int main(int argc, char* argv[]) {

    Uint32 timeStart, timeEnd, curTime, prevTime;
    int frameTime = 1000 / FPS;
    int fallingTime = 500;
    int minosType, minosAngle, nextType, nextAngle, removeCount;
    int refpos[26][14];
    MINOS curMinos, nextMinos;
    vector tetrisCord, tmpCord, displayCord;
    int menuFocus = 0;
    char scoreString[SCORE_LENGTH] = "0000000000\0";
    int score = 0;

    SDL_Window* window;
    GAME tetris;
    SDL_Texture* ASCII[95];
    SDL_Texture* TitleTexture, * overTexture, * pauseTexture;
    TTF_Font* TitleFont, * MainOptionFont;
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
    tetris.state = IN_MENU;
    TitleFont = TTF_OpenFont("assets/fonts/BasculaCollege.ttf", 128);
    MainOptionFont = TTF_OpenFont("assets/fonts/NotoSansMonoCJKsc-Regular.otf", 128);

    TitleTexture = getFontTexture(tetris.renderer, TitleFont, menuFg, "Tetris");
    for (int i = 0; i < 95; i++) {
        ASCII[i] = getCharTexture(tetris.renderer, MainOptionFont, menuFg, i + 32);
    }
    overTexture = getFontTexture(tetris.renderer, TitleFont, (SDL_Color) {0, 0, 0, 255}, "GAME OVER");
    pauseTexture = getFontTexture(tetris.renderer, TitleFont, (SDL_Color) { 125, 125, 125, 255 }, "  PAUSE  ");

    /* Game Loop */
    while (tetris.state != QUIT) {

        timeStart = SDL_GetTicks();

        SDL_Event event;
        switch (tetris.state) {
        case IN_MENU:

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
            for (int i = 0; i < 3; i++) {
                if (i == menuFocus) {
                    size_t optLen = strlen(menuSelect[i]);
                    renderText(tetris.renderer, menuSelect[i], ASCII, (WIN_WIDTH - optLen * CHAR_WIDTH) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }
                else {
                    size_t optLen = strlen(menuOption[i]);
                    renderText(tetris.renderer, menuOption[i], ASCII, (WIN_WIDTH - optLen * CHAR_WIDTH) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }
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
            renderText(tetris.renderer, scoreString, ASCII, tetrisWin_x, 10, 12, 24);
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

            convertScore(scoreString, score);

            SDL_SetRenderDrawColor(tetris.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(tetris.renderer);
            renderText(tetris.renderer, scoreString, ASCII, 10, 10, 12, 24);
            break;
        }

        /* Limit Frame */
        timeEnd = SDL_GetTicks();
        if ((timeEnd - timeStart) < frameTime)
            SDL_Delay(frameTime - (timeEnd - timeStart));

        /* Present Renderer */
        SDL_RenderPresent(tetris.renderer);
    }

    TTF_CloseFont(TitleFont);
    TTF_CloseFont(MainOptionFont);
    SDL_DestroyRenderer(tetris.renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}