#ifndef OCTIS_H
#define OCTIS_H

#define WIDTH 8
#define HEIGHT 15

#define BLOCK_COUNT 13

#define MAX_BLOCK_SIZE 5
#define MAX_BLOCK_COUNT 3

#define OFF_VALUE 0

#define CHANCES_TOTAL 1000
#define RARE 40
#define COMMON 88

typedef struct {
    bool block[MAX_BLOCK_SIZE][MAX_BLOCK_SIZE];
    int width;
    int height;
    int count;
    int color;
    int chance;
} BLOCK;

void startGame();

void createBoard(int board[][WIDTH], const int width, const int height, const int baseValue);
void pickBlocks(BLOCK *blocks, int blockCount);
BLOCK *turn(int board[][WIDTH], BLOCK *playerBlocks, int *blocksAmount);

int readIntFromUser(const char *prompt, int minimum, int maximum);

void rotate(BLOCK *block);
void flip(BLOCK *block);
bool compareBlock(BLOCK *blockA, BLOCK *blockB);
void copyBlock(BLOCK *blockA, BLOCK *blockB);
void copyBoard(int boardA[][WIDTH], int boardB[][WIDTH]);
int countBoardSquares(int board[][WIDTH], int offValue);
void addBlock(int board[][WIDTH], BLOCK *block, int X, int Y);
bool fall(int board[][WIDTH], BLOCK *block, int X);
bool isLineFinished(int *line, int offValue);
void removeCompletedLines(int board[][WIDTH]);

void printBlocks(BLOCK *blocks, int blocksAmount);
void printBlock(BLOCK block);
void printBlockOffset(BLOCK block, int offset);
char* multiplyChar(char character, int amount);
void printColored(int color, const char *text);
void printBoard(int board[][WIDTH]);


#endif