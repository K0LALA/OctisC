#include "octis.h"

#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define BLOCK_SIZE 50
#define BLOCK_BORDER_SIZE 10

#define BLOCKS_WIDTH 250
#define BLOCKS_HEIGHT 950
#define BLOCKS_MARGIN 100

#define BORDER_SIZE 5

#define FONT_NOTO_SANS "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"

void init();
void finish();

void updateScreen();
void clearScreen();
void renderOnMainTexture();
void renderPresentFromTexture();
void renderScreen(int board[][WIDTH], BLOCK *firstBlocks, int firstBlocksAmount, BLOCK *secondBlocks, int secondBlocksAmount, bool firstPlayerToPlay);

int renderText(const char *text, SDL_Color color, int x, int y, int fontSize);
void renderBoard(int board[][WIDTH], int x, int y);
void renderBlocks(BLOCK* blocks, int blocksAmount, int x, int y);

#endif