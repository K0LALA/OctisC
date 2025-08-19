#ifndef OCTIS_H
#define OCTIS_H

#define MAX_BLOCK_SIZE 5
#define MAX_BLOCK_COUNT 3

#define OFF_VALUE 0

typedef struct {
    bool block[MAX_BLOCK_SIZE][MAX_BLOCK_SIZE];
    int width;
    int height;
    int count;
    int color;
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
int countBoardSquares(int *board, int offValue);
void addBlock(int *board, BLOCK *block, int X, int Y);
bool fall(int *board, BLOCK *block, int X);
bool isLineFinished(int *line, int offValue);
void removeCompletedLines(int *board);

void printBlocks(BLOCK *blocks, int blocksAmount);
void printBlock(BLOCK block);
void printBlockOffset(BLOCK block, int offset);
char* multiplyChar(char character, int amount);
void printColored(int color, const char *text);
void printBoard(int *board);


#endif