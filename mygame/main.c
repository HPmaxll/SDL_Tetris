#include <stdio.h>
#include <string.h>
#include <io.h>
#include "tetris.h"
#include "ui.h"
#include "config.h"
#include "constants.h"

char* mainMenu[] = {
    "Start",
    "Option",
    "Quit"
};

char* optMenu[] = {
    "Color Scheme:",
    "Start Level:",
    "Grid:"
};

char* pauseMenu[] = {
    "Continue",
    "Return To Title",
    "Quit"
};

char* switchStr[] = {
    "On",
    "Off"
};

const int fallSpeed[9] = {
    500, 400, 300, 250, 200, 150, 100, 80, 50
};

char scoreString[] = "Score: 0000000000\0";
char levelString[] = "Level: 1\0";

SDL_Color menuFg = { 0, 0, 100, 255 };
SDL_Rect menuWin = { (WIN_WIDTH - 361) / 2, 80, 361, 128 };

int main(int argc, char* argv[]) {

    ITEM tetris_config[OPTSIZE];
    memcpy(tetris_config, default_config, OPTSIZE * sizeof(ITEM));
    if (_access(CONFIG_FILE, 0) == 0)
        parse_config(tetris_config);
    else
        save_config(tetris_config);
    
    int startLevel = tetris_config[STARTLEVEL].value + 1;
    int gridState = tetris_config[GRID].value;
    int colorScheme = tetris_config[COLORSCHEME].value;
    int newScheme, prevScheme;
    SCHEME currentScheme = scheme[colorScheme];
    int FPS = 60;
    int menuFocus = 0;
    int optFocus = 0;
    int pauseFocus = 0;
    int menuID = 0;
    long score = 0;
    int flashFlag = 0;
    Uint32 frameStart, frameEnd, curTime, prevTime, flashStart, flashEnd;
    int frameTime = 1000 / FPS;
    
    int minosType, minosAngle, nextType, nextAngle, removeCount, level = startLevel;
    long lineCount = 0;
    int refpos[26][14];
    MINOS curMinos, nextMinos;
    vector tetrisCord, tmpCord, displayCord;
    
    SDL_Window * window = NULL;
    GAME tetris;
    SDL_Texture* ASCII[95] = { NULL };
    SDL_Texture * TitleTexture = NULL, * overTexture = NULL, * pauseTexture = NULL, * focusLeft = NULL, * focusRight = NULL;
    TTF_Font * TitleFont, * MainOptionFont, * subTitleFont;

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
    subTitleFont = TTF_OpenFont("assets/fonts/BasculaCollege.ttf", 48);
    MainOptionFont = TTF_OpenFont("assets/fonts/NotoSansMonoCJKsc-Regular.otf", 20);

    TitleTexture = getFontTexture(tetris.renderer, TitleFont, currentScheme.main_fg, "Tetris");
    for (int i = 0; i < 95; i++) {
        ASCII[i] = getCharTexture(tetris.renderer, MainOptionFont, currentScheme.menu_fg, i + 32);
    }
    overTexture = getFontTexture(tetris.renderer, subTitleFont, currentScheme.main_fg, "GAME OVER");
    pauseTexture = getFontTexture(tetris.renderer, subTitleFont, currentScheme.main_fg, "  PAUSE  ");

    TTF_CloseFont(TitleFont);
    TTF_CloseFont(subTitleFont);
    TTF_CloseFont(MainOptionFont);

    focusLeft = loadImageFile(tetris.renderer, "assets/pics/left.png");
    focusRight = loadImageFile(tetris.renderer, "assets/pics/right.png");

    tetris.state = IN_MENU;
    SDL_SetRenderDrawBlendMode(tetris.renderer, SDL_BLENDMODE_BLEND);
    /* Game Loop */
    while (tetris.state != QUIT) {

        frameStart = SDL_GetTicks();

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
                                levelString[7] = startLevel + '0';
                                level = startLevel;
                                lineCount = 0;
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

                                initScore(scoreString);
                                score = 0;
                                break;
                            case 1:
                                menuID = 1;
                                newScheme = prevScheme = colorScheme;
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

                setDrawColor(tetris.renderer, currentScheme.main_bg);
                SDL_RenderClear(tetris.renderer);
                SDL_RenderCopy(tetris.renderer, TitleTexture, NULL, &menuWin);
                for (int i = 0; i < MENU_COUNT; i++) {
                    size_t optLen = strlen(mainMenu[i]);
                    renderText(tetris.renderer, mainMenu[i], ASCII, (WIN_WIDTH - optLen * CHAR_WIDTH) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }
                SDL_RenderCopy(tetris.renderer, focusLeft, NULL, &(SDL_Rect) {WIN_WIDTH - 230, 40 * menuFocus + 245, 23, 23});
                SDL_RenderCopy(tetris.renderer, focusRight, NULL, &(SDL_Rect) {206, 40 * menuFocus + 245, 23, 23});
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
                                newScheme ^= 1;
                                break;
                            case 1:
                                if (startLevel > 1)
                                    startLevel -= 1;
                                break;
                            case 2:
                                gridState ^= 1;
                                break;
                            }
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            switch (optFocus) {
                            case 0:
                                newScheme ^= 1;
                                break;
                            case 1:
                                if (startLevel < 9)
                                    startLevel += 1;
                                break;
                            case 2:
                                gridState ^= 1;
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
                            colorScheme = newScheme;
                            currentScheme = scheme[colorScheme];
                            optFocus = 0;
                            menuID = 0;
                            break;
                        }
                        break;
                    case SDL_QUIT:
                        tetris.state = QUIT;
                        break;
                    }
                }
                if (colorScheme != prevScheme) {
                    SDL_DestroyTexture(TitleTexture);
                    SDL_DestroyTexture(overTexture);
                    SDL_DestroyTexture(pauseTexture);
                    for (int i = 0; i < 95; i++)
                        SDL_DestroyTexture(ASCII[i]);

                    TitleFont = TTF_OpenFont("assets/fonts/BasculaCollege.ttf", 128);
                    subTitleFont = TTF_OpenFont("assets/fonts/BasculaCollege.ttf", 48);
                    MainOptionFont = TTF_OpenFont("assets/fonts/NotoSansMonoCJKsc-Regular.otf", 20);

                    TitleTexture = getFontTexture(tetris.renderer, TitleFont, currentScheme.main_fg, "Tetris");
                    overTexture = getFontTexture(tetris.renderer, subTitleFont, currentScheme.main_fg, "GAME OVER");
                    pauseTexture = getFontTexture(tetris.renderer, subTitleFont, currentScheme.main_fg, "  PAUSE  ");
                    for (int i = 0; i < 95; i++)
                        ASCII[i] = getCharTexture(tetris.renderer, MainOptionFont, currentScheme.menu_fg, i + 32);
                    

                    TTF_CloseFont(TitleFont);
                    TTF_CloseFont(subTitleFont);
                    TTF_CloseFont(MainOptionFont);
                }
                setDrawColor(tetris.renderer, currentScheme.main_bg);
                SDL_RenderClear(tetris.renderer);
                SDL_RenderCopy(tetris.renderer, TitleTexture, NULL, &menuWin);
                
                SDL_SetRenderDrawColor(tetris.renderer, 160, 160, 160, 255);
                SDL_RenderFillRect(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2 + 5, 225, 320, 160});
                
                drawRectWithBorder(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2, 220, 320, 160}, currentScheme.menu_bg, currentScheme.border);
                
                for (int i = 0; i < MENU_COUNT; i++) {
                    size_t optLen = strlen(optMenu[i]);
                    renderText(tetris.renderer, optMenu[i], ASCII, (WIN_WIDTH - 300) / 2, 40 * i + 240, CHAR_WIDTH, CHAR_HEIGHT);
                }

                drawRectWithBorder(tetris.renderer, &(SDL_Rect){356, 245, 25, 25}, SchemeSign[newScheme], currentScheme.border);
                renderText(tetris.renderer, (char[2]) { startLevel + '0', '\0' }, ASCII, 361, 280, CHAR_WIDTH, CHAR_HEIGHT);
                renderText(tetris.renderer, switchStr[gridState], ASCII, 356, 320, CHAR_WIDTH, CHAR_HEIGHT);

                SDL_RenderCopy(tetris.renderer, focusLeft, NULL, &(SDL_Rect) {325, 40 * optFocus + 245, 23, 23});
                SDL_RenderCopy(tetris.renderer, focusRight, NULL, &(SDL_Rect) {390, 40 * optFocus + 245, 23, 23});
                break;
            }
            break;

        case GAME_RUN:
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
                    case SDL_SCANCODE_ESCAPE:
                        tetris.state = GAME_PAUSE;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            curTime = SDL_GetTicks();
            if (curTime - prevTime >= fallSpeed[level - 1]) {
                tetrisCord.y += 1;
                prevTime = curTime;
            }
            if (minosCollide(&tetrisCord, refpos, &curMinos)) {
                tetrisCord.y -= 1;
                setAxis(&tetrisCord, &curMinos, refpos);
                removeCount = unsetAxis(refpos);
                lineCount += removeCount;
                if (lineCount / 15 > level - 1) {
                    level += 1;
                    levelString[7] = level + '0';
                }
                score += removeCount * removeCount * level * 100;
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
            if (checkGameOver(refpos)) {
                tetris.state = GAME_OVER;
                lineCount = 0;
                flashStart = SDL_GetTicks();
            }

            setDrawColor(tetris.renderer, currentScheme.main_bg);
            SDL_RenderClear(tetris.renderer);
            renderText(tetris.renderer, levelString, ASCII, tetrisWin_x - 10, 10, 10, 30);
            renderText(tetris.renderer, scoreString, ASCII, tetrisWin_x + 90, 10, 10, 30);
            drawRectWithBorder(tetris.renderer, &tetrisWin, currentScheme.game_bg, currentScheme.border);
            if (gridState == 0)
                drawGrid(tetris.renderer, currentScheme.grid);
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
                        pauseFocus -= 1;
                        if (pauseFocus < 0)
                            pauseFocus = 2;
                        break;
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        pauseFocus += 1;
                        if (pauseFocus > 2)
                            pauseFocus = 0;
                        break;
                    case SDL_SCANCODE_RETURN:
                        switch (pauseFocus) {
                        case 0:
                            tetris.state = GAME_RUN;
                            break;
                        case 1:
                            tetris.state = IN_MENU;
                            break;
                        case 2:
                            tetris.state = QUIT;
                            break;
                        }
                        pauseFocus = 0;
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        pauseFocus = 0;
                        tetris.state = GAME_RUN;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            setDrawColor(tetris.renderer, currentScheme.main_bg);
            SDL_RenderClear(tetris.renderer);
            renderText(tetris.renderer, levelString, ASCII, tetrisWin_x - 10, 10, 10, 30);
            renderText(tetris.renderer, scoreString, ASCII, tetrisWin_x + 90, 10, 10, 30);
            drawRectWithBorder(tetris.renderer, &tetrisWin, currentScheme.game_bg, currentScheme.border);
            if (gridState == 0)
                drawGrid(tetris.renderer, currentScheme.grid);
            drawRecord(tetris.renderer, refpos);
            drawMinos(tetris.renderer, &displayCord, &nextMinos);
            drawMinos(tetris.renderer, &tetrisCord, &curMinos);

            drawRectWithBorder(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2, 220, 320, 200}, currentScheme.menu_bg, currentScheme.border);
            SDL_RenderCopy(tetris.renderer, pauseTexture, NULL, &(SDL_Rect) {(WIN_WIDTH - 254) / 2, 235, 254, 48});
            for (int i = 0; i < MENU_COUNT; i++) {
                size_t optLen = strlen(pauseMenu[i]);
                renderText(tetris.renderer, pauseMenu[i], ASCII, (WIN_WIDTH - optLen * CHAR_WIDTH) / 2, 40 * i + 285, CHAR_WIDTH, CHAR_HEIGHT);
            }
            SDL_RenderCopy(tetris.renderer, focusLeft, NULL, &(SDL_Rect) {380, 40 * pauseFocus + 290, 23, 23});
            SDL_RenderCopy(tetris.renderer, focusRight, NULL, &(SDL_Rect) {(WIN_WIDTH - 260) / 2, 40 * pauseFocus + 290, 23, 23});
            break;

        case GAME_OVER:
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    tetris.state = IN_MENU;
                    level = 1;
                    levelString[7] = level + '0';
                    break;
                case SDL_QUIT:
                    tetris.state = QUIT;
                    break;
                }
            }

            flashEnd = SDL_GetTicks();
            if (flashEnd - flashStart > 500) {
                flashFlag ^= 1;
                flashStart = flashEnd;
            }

            setDrawColor(tetris.renderer, currentScheme.main_bg);
            SDL_RenderClear(tetris.renderer);
            renderText(tetris.renderer, levelString, ASCII, tetrisWin_x - 10, 10, 10, 30);
            renderText(tetris.renderer, scoreString, ASCII, tetrisWin_x + 90, 10, 10, 30);
            drawRectWithBorder(tetris.renderer, &tetrisWin, currentScheme.game_bg, currentScheme.border);
            if (gridState == 0)
                drawGrid(tetris.renderer, currentScheme.grid);
            drawRecord(tetris.renderer, refpos);
            drawRectWithBorder(tetris.renderer, &(SDL_Rect) {(WIN_WIDTH - 320) / 2, 220, 320, 200}, currentScheme.menu_bg, currentScheme.border);
            SDL_RenderCopy(tetris.renderer, overTexture, NULL, &(SDL_Rect) {(WIN_WIDTH - 305) / 2, (WIN_WIDTH - 48) / 2, 305, 48});
            if (flashFlag)
                renderText(tetris.renderer, "Press Any Key To Continue", ASCII, (WIN_WIDTH - 260) / 2, (WIN_WIDTH - 48) / 2 + 100, 10, 30);
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
      
            break;
        }

        /* Limit Frame */
        frameEnd = SDL_GetTicks();
        if ((frameEnd - frameStart) < frameTime)
            SDL_Delay(frameTime - (frameEnd - frameStart));

        /* Present Renderer */
        SDL_RenderPresent(tetris.renderer);
    }
    tetris_config[STARTLEVEL].value = startLevel - 1;
    tetris_config[STARTLEVEL].valueStr = config_opt[STARTLEVEL].value[startLevel - 1];
    tetris_config[GRID].value = gridState;
    tetris_config[GRID].valueStr = config_opt[GRID].value[gridState];
    tetris_config[COLORSCHEME].value = colorScheme;
    tetris_config[COLORSCHEME].valueStr = config_opt[COLORSCHEME].value[colorScheme];
    save_config(tetris_config);
    SDL_DestroyRenderer(tetris.renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}