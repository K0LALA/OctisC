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
     RARE,
     true},

    {{{1, 1, 0},
      {0, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     COMMON,
     true},

    {{{1, 1, 1, 1}},
     4,
     1,
     4,
     COMMON,
     true},

    {{{1, 0, 0},
      {1, 1, 1}},
     3,
     2,
     4,
     COMMON,
     true},

    {{{0, 1},
      {0, 1},
      {1, 1},
      {1, 0}},
     2,
     4,
     5,
     COMMON,
     true},

    {{{1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}},
     3,
     3,
     8,
     RARE,
     true},

    {{{0, 1, 1},
      {1, 1, 0}},
     3,
     2,
     4,
     COMMON,
     true},

    {{{1, 0},
      {1, 1},
      {1, 0}},
     2,
     3,
     4,
     COMMON,
     true},

    {{{1, 0, 1},
      {1, 1, 1}},
     3,
     2,
     5,
     COMMON,
     true},

    {{{0, 0, 1},
      {0, 1, 1},
      {1, 1, 0}},
     3,
     3,
     5,
     COMMON,
     true},

    {{{0, 1, 0},
      {1, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     COMMON,
     true},

    {{{1, 1, 0},
      {0, 1, 1}},
     3,
     2,
     4,
     COMMON,
     true},

    {{{0, 1, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 1, 0},
      {1, 0, 0}},
     3,
     5,
     8,
     RARE,
     true}};

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

    srand(time(NULL));
    startGame();

    return 0;
}

void startGame()
{
    bool firstPlayerToPlay = false;
    BLOCK *firstPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    int firstPlayerBlocksAmount = MAX_BLOCK_COUNT;
    pickBlocks(firstPlayerBlocks, MAX_BLOCK_COUNT);
    BLOCK *secondPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    int secondPlayerBlocksAmount = MAX_BLOCK_COUNT;
    pickBlocks(secondPlayerBlocks, MAX_BLOCK_COUNT);
    int *board = (int *)malloc(WIDTH * HEIGHT * sizeof(int));
    // int **board;
    // ASCII Table: 32 for spaces, 35 for #
    createBoard(board, WIDTH, HEIGHT, OFF_VALUE);

    do
    {
        firstPlayerToPlay = !firstPlayerToPlay;
        printf("Player %d to play!\n", !firstPlayerToPlay + 1);

        // Check if block list is empty for both players
        if (firstPlayerBlocksAmount <= 0)
        {
            pickBlocks(firstPlayerBlocks, MAX_BLOCK_COUNT);
            firstPlayerBlocksAmount = MAX_BLOCK_COUNT;
        }
        if (secondPlayerBlocksAmount <= 0)
        {
            pickBlocks(secondPlayerBlocks, MAX_BLOCK_COUNT);
            secondPlayerBlocksAmount = MAX_BLOCK_COUNT;
        }
    } while (turn(board, firstPlayerToPlay ? firstPlayerBlocks : secondPlayerBlocks, firstPlayerToPlay ? &firstPlayerBlocksAmount : &secondPlayerBlocksAmount));

    printf("\nPlayer %d lost!\n", !firstPlayerToPlay + 1);

    free(firstPlayerBlocks);
    free(secondPlayerBlocks);
    free(board);
}

void createBoard(int *board, int width, int height, int baseValue)
{
    int *ptr = board;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            *ptr = baseValue;
            ptr++;
        }
    }
}

void pickBlocks(BLOCK *blocks, const int blockCount)
{
    // TODO: Apply Rarity
    for (int i = 0; i < blockCount; i++)
    {
        int randomValue = rand();
        int blockIndex = randomValue % BLOCK_COUNT;
        bool doFlip = randomValue % 2 == 0;
        int rotationCount = randomValue % 3;

        blocks[i] = BLOCKS[blockIndex];
        if (doFlip)
            flip(&blocks[i]);
        for (int i = 0; i < rotationCount; i++)
            rotate(&blocks[i]);
    }
}

/// @brief Gets the input from the player to get his moves
/// @param board The current board
/// @param playerBlocks The player's blocks
/// @return 0 if the player lost, true if the game can continue
bool turn(int *board, BLOCK *playerBlocks, int *blocksAmount)
{
    // Print the available blocks
    printBlocks(playerBlocks, MAX_BLOCK_COUNT);

    // Get the chosen block from the player
    char *prompt = (char *)malloc(sizeof(char) * (26 + (int)(*blocksAmount / 10)));
    sprintf(prompt, "Choose your block [1-%d]: ", *blocksAmount);
    int blockIndex = readIntFromUser(prompt, 1, *blocksAmount);
    BLOCK block;
    int attempt = 0;
    do
    {
        block = playerBlocks[blockIndex - 1 + attempt];
        if (block.available)
        {
            break;
        }
        else
        {
            attempt++;
        }
    } while (attempt < MAX_BLOCK_COUNT);

    if (attempt >= MAX_BLOCK_COUNT)
    {
        puts("Couldn't get choosen block from the player");
        free(prompt);
        return false;
    }

    // Print the chosen block above the board
    BLOCK *showedBlocks = (BLOCK *)malloc(sizeof(BLOCK));
    showedBlocks[0] = block;
    unsigned short rotatedBlocks = 1;
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

    // Get the chosen orientation from the player
    if (rotatedBlocks > 1)
    {
        printBlocks(showedBlocks, rotatedBlocks);
        prompt = (char *)realloc(prompt, sizeof(char) * 36);
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
    if (!fall(board, &block, chosenX - 1))
    {
        return false;
    }

    // Remove completed lines
    removeCompletedLines(board);

    // Print the board
    printBoard(board);

    // Remove the block from the list
    (*blocksAmount)--;
    playerBlocks[blockIndex - 1 + attempt].available = false;

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
    blockA->available = blockB->available;
    // Not really necessary since it's not used after block choosing
    // blockA->chance = blockB->chance;
}

void copyBoard(int *boardA, int *boardB)
{
    int *ptrA = boardA;
    int *ptrB = boardB;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            *ptrA = *ptrB;
            ptrA++;
            ptrB++;
        }
    }
}

int countBoardSquares(int *board, int onValue)
{
    int *ptr = board;
    int count = 0;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (*ptr == onValue)
                count++;
            ptr++;
        }
    }
    return count;
}

void addBlock(int *board, BLOCK *block, int X, int Y, int onValue)
{
    int *ptr = board;
    ptr += Y * WIDTH + X;
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            if (block->block[y][x])
            {
                *ptr = onValue;
            }
            ptr++;
        }
        ptr += WIDTH - block->width;
    }
}

bool fall(int *board, BLOCK *block, int X)
{
    int totalSquares = block->count + countBoardSquares(board, ON_VALUE);
    int height = 0;
    while (height + block->height <= HEIGHT)
    {
        int *boardCopy = (int *)malloc(WIDTH * HEIGHT * sizeof(int));
        // createBoard(boardCopy, WIDTH, HEIGHT, OFF_VALUE);
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
        free(boardCopy);
    }
    addBlock(board, block, X, height - 1, ON_VALUE);
    return true;
}

bool isLineFinished(int *line, int onValue)
{
    for (int x = 0; x < WIDTH; x++)
    {
        if (line[x] != onValue)
        {
            return false;
        }
    }
    return true;
}

void removeCompletedLines(int *board)
{
    int *start = board;
    int *ptr = board;
    for (int y = 0; y < HEIGHT; y++)
    {
        if (isLineFinished(ptr, ON_VALUE))
        {
            int *ptrCopy = ptr;
            for (int i = y; i > 0; i--)
            {
                // board[i] = board[i - 1];
                int *ptrAbove = ptr - WIDTH;
                for (int x = 0; x < WIDTH; x++)
                {
                    *ptrCopy = *ptrAbove;
                    ptrCopy--;
                    ptrAbove--;
                }
            }
            for (int x = 0; x < WIDTH; x++)
            {
                *start = OFF_VALUE;
                start++;
            }
        }
        ptr += WIDTH;
    }
}

void printBlocks(BLOCK *blocks, int blocksAmount)
{
    char spacing[6] = "     ";
    for (int y = 0; y < MAX_BLOCK_HEIGHT; y++)
    {
        for (int i = 0; i < blocksAmount; i++)
        {
            if (!blocks[i].available)
            {
                continue;
            }
            if (y >= blocks[i].height)
            {
                char *space = (char *)malloc(sizeof(char) * (2 * blocks[i].width + 7));
                space = multiplyChar(' ', 2 * blocks[i].width + 6);
                printf("%s", space);
                free(space);
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
    char *multipliedChar = (char *)malloc(sizeof(char) * (amount + 1));
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

void printBoard(int *board)
{
    int *ptr = board;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            printf("%c ", (char)*ptr);
            ptr++;
        }
        printf("%d\n", HEIGHT - y);
    }

    for (int x = 1; x <= WIDTH; x++)
    {
        printf("%d ", x);
    }
    printf("\n");
}
