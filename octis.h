#ifndef OCTIS_H
#define OCTIS_H

#define MAX_BLOCK_HEIGHT 5
#define MAX_BLOCK_WIDTH 5
#define MAX_BLOCK_COUNT 3

typedef struct {
    bool block[MAX_BLOCK_HEIGHT][MAX_BLOCK_WIDTH];
    int width;
    int height;
    float chance;
} BLOCK;

void startGame();

void createBoard(int **board, const int width, const int height, const int baseValue);
void createEmptyBlock(bool **block, int width, int height);
void pickBlocks(BLOCK *firstPlayerBlocks, BLOCK *secondPlayerBlocks, const int blockCount);
bool turn(int **board, BLOCK *playerBlocks, int blocksAmount);

int readIntFromUser(const char *prompt, int minimum, int maximum);

void rotate(BLOCK *block);
void flip(BLOCK *block);

void printBlocks(BLOCK *blocks, int blocksAmount);
void printBlock(const BLOCK block);
char* multiplyChar(char character, int amount);
void printColored(int color, const char *text);
void printBoard(int **board, const int width, const int height);


#endif