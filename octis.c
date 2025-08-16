#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "octis.h"

const int BLOCK_COUNT = 13;

const float RARE = 0.04f;
const float COMMON = 0.088f;

const BLOCK BLOCKS[] = {
    {{{1}},
     1,
     1,
     1,
     RARE},

    {{{1, 1, 0},
      {0, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     COMMON},

    {{{1, 1, 1, 1}},
     4,
     1,
     4,
     COMMON},

    {{{1, 0, 0},
      {1, 1, 1}},
     3,
     2,
     4,
     COMMON},

    {{{0, 1},
      {0, 1},
      {1, 1},
      {1, 0}},
     2,
     4,
     5,
     COMMON},

    {{{1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}},
     3,
     3,
     8,
     RARE},

    {{{0, 1, 1},
      {1, 1, 0}},
     3,
     2,
     4,
     COMMON},

    {{{1, 0},
      {1, 1},
      {1, 0}},
     2,
     3,
     4,
     COMMON},

    {{{1, 0, 1},
      {1, 1, 1}},
     3,
     2,
     5,
     COMMON},

    {{{0, 0, 1},
      {0, 1, 1},
      {1, 1, 0}},
     3,
     3,
     5,
     COMMON},

    {{{0, 1, 0},
      {1, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     COMMON},

    {{{1, 1, 0},
      {0, 1, 1}},
     3,
     2,
     4,
     COMMON},

    {{{0, 1, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 1, 0},
      {1, 0, 0}},
     3,
     5,
     8,
     RARE}};

const int WIDTH = 8;
const int HEIGHT = 15;

int main(int argc, char **argv)
{
    printColored(34, "Welcome to Octis!\n");
    printf(" _____     _   _     \n");
    printf("|     |___| |_|_|___ \n");
    printf("|  |  |  _|  _| |_ -|\n");
    printf("|_____|___|_| |_|___|\n");
    printf("This game is a 2-player Tetris game.\n");
    printf("In order to win, the oponent has to place a block over the limit.\n");

    // printf("\033[%dm%s\033[0m\n", 32, "Press Enter to start the game");
    /*char *scanTest = (char *) malloc(sizeof(char) * 10);
    scanf("%s", scanTest);
    printf("%s\n", scanTest);*/

    srand(time(NULL));
    startGame();

    return 0;
}

void startGame()
{
    bool firstPlayerToPlay = false;
    BLOCK *firstPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    int firstPlayerBlocksAmount = MAX_BLOCK_COUNT;
    BLOCK *secondPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    int secondPlayerBlocksAmount = MAX_BLOCK_COUNT;
    pickBlocks(firstPlayerBlocks, secondPlayerBlocks, MAX_BLOCK_COUNT);
    int **board = (int **)malloc(WIDTH * HEIGHT * sizeof(int));
    // ASCII Table: 32 for spaces, 35 for #
    createBoard(board, WIDTH, HEIGHT, OFF_VALUE);

    do
    {
        firstPlayerToPlay = !firstPlayerToPlay;
        printf("Player %d to play!\n", !firstPlayerToPlay + 1);

        // Check if block list is empty for both players

    } while (turn(board, firstPlayerToPlay ? firstPlayerBlocks : secondPlayerBlocks, firstPlayerToPlay ? firstPlayerBlocksAmount : secondPlayerBlocksAmount));

    printf("\nPlayer %d lost!\n", !firstPlayerToPlay + 1);

    free(firstPlayerBlocks);
    free(secondPlayerBlocks);
    free(board);
}

void createBoard(int **board, int width, int height, int baseValue)
{
    for (int y = 0; y < height; y++)
    {
        int *line = (int *)malloc(sizeof(int) * width);
        for (int x = 0; x < width; x++)
        {
            line[x] = baseValue;
        }
        board[y] = line;
    }
}

void createEmptyBlock(bool **block, int width, int height)
{
    bool *line = (bool *)malloc(sizeof(bool) * width);
    for (int x = 0; x < width; x++)
    {
        line[x] = false;
    }
    for (int y = 0; y < height; y++)
    {
        block[y] = line;
    }
}

void pickBlocks(BLOCK *firstPlayerBlocks, BLOCK *secondPlayerBlocks, const int blockCount)
{
    // TODO: Apply Rarity

    // For each player, pick three random numbers
    // Map each of those numbers to the range [0, BLOCK_COUNT)
    // and assign the corresponding blocks to the players
    for (int i = 0; i < blockCount; i++)
    {
        int firstPlayerRand = rand();
        int firstPlayerIndex = firstPlayerRand % BLOCK_COUNT;
        bool firstPlayerFlip = firstPlayerRand % 2 == 0;
        int firstPlayerRotations = firstPlayerRand % 3;

        BLOCK firstPlayerBlock = BLOCKS[firstPlayerIndex];
        if (firstPlayerFlip)
            flip(&firstPlayerBlock);
        for (int i = 0; i < firstPlayerRotations; i++)
            rotate(&firstPlayerBlock);
        firstPlayerBlocks[i] = firstPlayerBlock;

        int secondPlayerRand = rand();
        int secondPlayerIndex = secondPlayerRand % BLOCK_COUNT;
        bool secondPlayerFlip = secondPlayerRand % 2 == 0;
        int secondPlayerRotations = secondPlayerRand % 3;

        BLOCK secondPlayerBlock = BLOCKS[secondPlayerIndex];
        if (secondPlayerFlip)
            flip(&secondPlayerBlock);
        for (int i = 0; i < secondPlayerRotations; i++)
            rotate(&secondPlayerBlock);
        secondPlayerBlocks[i] = secondPlayerBlock;
    }
}

/// @brief Gets the input from the player to get his moves
/// @param board The current board
/// @param playerBlocks The player's blocks
/// @return 0 if the player lost, true if the game can continue
bool turn(int **board, BLOCK *playerBlocks, int blocksAmount)
{
    // Print the available blocks
    printBlocks(playerBlocks, 3);

    // Get the chosen block from the player
    char *prompt = (char *)malloc(sizeof(char) * (25 + (int)(blocksAmount / 10)));
    sprintf(prompt, "Choose your block [1-%d]: ", blocksAmount);
    int blockIndex = readIntFromUser(prompt, 1, blocksAmount);
    BLOCK block = playerBlocks[blockIndex - 1];

    // Print the chosen block above the board
    BLOCK *showedBlocks = (BLOCK *)malloc(sizeof(BLOCK));
    showedBlocks[0] = block;
    int rotatedBlocks = 1;
    for (int i = 0; i < 3; i++)
    {
        BLOCK *rotatedBlock = (BLOCK *)malloc(sizeof(BLOCK));
        copyBlock(rotatedBlock, &showedBlocks[rotatedBlocks - 1]);
        rotate(rotatedBlock);
        if (compareBlock(rotatedBlock, &block))
        {
            if (i == 0)
            {
                break;
            }
            continue;
        }
        rotatedBlocks++;
        showedBlocks = (BLOCK *)realloc(showedBlocks, sizeof(BLOCK) * rotatedBlocks);
        showedBlocks[rotatedBlocks - 1] = *rotatedBlock;
        free(rotatedBlock);
    }
    printBlocks(showedBlocks, rotatedBlocks);

    // Get the chosen orientation from the player
    if (rotatedBlocks > 1)
    {
        prompt = (char *)realloc(prompt, sizeof(char) * 32);
        sprintf(prompt, "Choose your orientation [1-%d]: ", rotatedBlocks);
        int chosenRotation = readIntFromUser(prompt, 1, rotatedBlocks);
        block = showedBlocks[chosenRotation - 1];
    }
    free(showedBlocks);

    // Print the rotated block above the board
    int middleX = WIDTH - block.width;
    printBlockOffset(block, middleX);
    printBoard(board);

    // Get the chosen abscissa
    int maxX = WIDTH - block.width + 1;
    prompt = (char *)realloc(prompt, sizeof(char) * (29 + (int)(maxX / 10)));
    sprintf(prompt, "Choose your abscissa [1-%d]: ", maxX);
    int chosenX = readIntFromUser(prompt, 1, maxX);
    free(prompt);

    // Place the block in the board
    bool result = fall(board, &block, chosenX - 1);

    // Remove completed lines

    // Print the board
    printBoard(board);

    // Remove the block from the list

    return true;
}

int readIntFromUser(const char *prompt, int minimum, int maximum)
{
    int readedValue;
    do
    {
        fputs(prompt, stdout);

        // Read from the user
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            puts("Failed to read from the user.");
            exit(EXIT_FAILURE);
        }

        // Convert char* to int
        readedValue = strtol(buffer, NULL, 10);
    } while (!(readedValue >= minimum && readedValue <= maximum));
    return readedValue;
}

void rotate(BLOCK *block)
{
    BLOCK tempBlock = *block;
    block->width = block->height;
    block->height = tempBlock.width;
    for (int y = 0; y < MAX_BLOCK_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_BLOCK_WIDTH; x++)
        {
            block->block[x][tempBlock.height - y - 1] = tempBlock.block[y][x];
        }
    }
}

void flip(BLOCK *block)
{
    BLOCK tempBlock = *block;
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            block->block[y][block->width - x - 1] = tempBlock.block[y][x];
        }
    }
}

bool compareBlock(BLOCK *blockA, BLOCK *blockB)
{
    for (int y = 0; y < MAX_BLOCK_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_BLOCK_WIDTH; x++)
        {
            if (blockA->block[y][x] != blockB->block[y][x])
                return false;
        }
    }
    return true;
}

void copyBlock(BLOCK *blockA, BLOCK *blockB)
{
    for (int y = 0; y < MAX_BLOCK_HEIGHT; y++)
    {
        for (int x = 0; x < MAX_BLOCK_WIDTH; x++)
        {
            blockA->block[y][x] = blockB->block[y][x];
        }
    }
    blockA->width = blockB->width;
    blockA->height = blockB->height;
    blockA->count = blockB->count;
    // blockA->chance = blockB->chance;
}

void copyBoard(int **boardA, int **boardB)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            boardA[y][x] = boardB[y][x];
        }
    }
}

int countBoardSquares(int **board, int onValue)
{
    int count = 0;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (board[y][x] == onValue)
                count++;
        }
    }
    return count;
}

void addBlock(int **board, BLOCK *block, int X, int Y, int onValue)
{
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            if (block->block[y][x])
            {
                // Changing one pixel changes the whole column
                board[Y + y][X + x] = onValue;
            }
        }
    }
}

bool fall(int **board, BLOCK *block, int X)
{
    int totalSquares = block->count + countBoardSquares(board, ON_VALUE);
    int height = 0;
    while (height + block->height <= HEIGHT)
    {
        int **boardCopy = (int **)malloc(WIDTH * HEIGHT * sizeof(int));
        createBoard(boardCopy, WIDTH, HEIGHT, OFF_VALUE);
        copyBoard(boardCopy, board);
        addBlock(boardCopy, block, X, height, ON_VALUE);
        if (totalSquares != countBoardSquares(boardCopy, ON_VALUE))
        {
            if (height - 1 < 0)
            {
                return false;
            }
            break;
        }
        height++;
    }
    addBlock(board, block, X, height - 1, ON_VALUE);
    return true;
}

void printBlocks(BLOCK *blocks, int blocksAmount)
{
    char spacing[6] = "     ";
    for (int y = 0; y < MAX_BLOCK_HEIGHT; y++)
    {
        for (int i = 0; i < blocksAmount; i++)
        {
            if (y >= blocks[i].height)
            {
                printf("%s%s", multiplyChar(' ', 2 * blocks[i].width + 1), spacing);
                continue;
            }
            for (int x = 0; x < blocks[i].width; x++)
            {
                if (blocks[i].block[y][x])
                {
                    printf("# ");
                }
                else
                {
                    printf("  ");
                }
            }
            if (i != blocksAmount - 1)
            {
                printf("%s ", spacing);
            }
        }
        printf("\n");
    }
}

void printBlock(const BLOCK block)
{
    printBlockOffset(block, 0);
}

void printBlockOffset(const BLOCK block, int offset)
{
    char *spacing = (char *)malloc(sizeof(char) * (offset + 1));
    spacing = multiplyChar(' ', offset);
    for (int y = 0; y < block.height; y++)
    {
        if (spacing != NULL)
            printf("%s", spacing);
        for (int x = 0; x < block.width; x++)
        {
            if (block.block[y][x])
            {
                printf("# ");
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
    free(spacing);
}

char *multiplyChar(char character, int amount)
{
    if (amount == 0)
    {
        return NULL;
    }
    char *multipliedChar = (char *)malloc(sizeof(char) * amount);
    for (int i = 0; i < amount; i++)
    {
        multipliedChar[i] = character;
    }
    return multipliedChar;
}

void printColored(int color, const char *text)
{
    printf("\033[%dm%s\033[0m", color, text);
}

void printBoard(int **board)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            printf("%c ", (char)board[y][x]);
        }
        printf("%d\n", HEIGHT - y);
    }

    for (int x = 1; x <= WIDTH; x++)
    {
        printf("%d ", x);
    }
    printf("\n");
}
