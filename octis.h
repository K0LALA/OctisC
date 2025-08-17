#ifndef OCTIS_H
#define OCTIS_H

#define MAX_BLOCK_HEIGHT 5
#define MAX_BLOCK_WIDTH 5
#define MAX_BLOCK_COUNT 3

#define OFF_VALUE 32
#define ON_VALUE 35

typedef struct {
    bool block[MAX_BLOCK_HEIGHT][MAX_BLOCK_WIDTH];
    int width;
    int height;
    int count;
    float chance;
} BLOCK;

void startGame();

void createBoard(int *board, const int width, const int height, const int baseValue);
void pickBlocks(BLOCK *blocks, int blockCount);
BLOCK *turn(int *board, BLOCK *playerBlocks, int *blocksAmount);

int readIntFromUser(const char *prompt, int minimum, int maximum);

void rotate(BLOCK *block);
void flip(BLOCK *block);
bool compareBlock(BLOCK *blockA, BLOCK *blockB);
void copyBlock(BLOCK *blockA, BLOCK *blockB);
void copyBoard(int *boardA, int *boardB);
void addBlock(int *board, BLOCK *block, int X, int Y, int onValue);
bool fall(int *board, BLOCK *block, int X);
bool isLineFinished(int *line, int onValue);
void removeCompletedLines(int *board);

void printBlocks(BLOCK *blocks, int blocksAmount);
void printBlock(const BLOCK block);
void printBlockOffset(const BLOCK block, int offset);
char* multiplyChar(char character, int amount);
void printColored(int color, const char *text);
void printBoard(int *board);


#endif