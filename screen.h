#include "octis.h"

#ifndef SCREEN_H
#define SCREEN_H

#define BORDER_SIZE 5
#define BLOCKS_MARGIN 30

#define FONT_NOTO_SANS "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"

void calculateSizes();
void init();
void updateRedrawPointers(BLOCK **p1Blocks, int *p1Amount, BLOCK **p2Blocks, int *p2Amount, bool *p1Play);
void updateRedrawBlockIndex(int *blockIndex);
void finish();

int handleEvents(SDL_Event event);

void updateScreen();
void clearScreen();
void renderOnMainTexture();
void renderPresentFromTexture();
int getWindowWidth();
int getWindowHeight();
void redraw(int board[][WIDTH]);
void renderScreen(int board[][WIDTH], BLOCK *firstBlocks, int firstBlocksAmount, BLOCK *secondBlocks, int secondBlocksAmount, bool firstPlayerToPlay);

int renderText(const char *text, SDL_Color color, int x, int y, int fontSize);
void renderBoard(int board[][WIDTH]);
void renderBlocksSelection(BLOCK *blocks, int blocksAmount, int blockIndex, bool firstPlayerToPlay);
void renderBlocks(BLOCK *blocks, int blocksAmount, bool firstPlayerToPlay);

#endif