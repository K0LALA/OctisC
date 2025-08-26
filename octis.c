#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <SDL.h>

#include "screen.h"
#include "octis.h"

// TODO: tmp
SDL_Event event;

const int BLOCK_COUNT = 13;

const float RARE = 0.04f;
const float COMMON = 0.088f;

const BLOCK BLOCKS[] = {
    {{{1}},
     1,
     1,
     1,
     31,
     RARE},

    {{{1, 1, 0},
      {0, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     31,
     COMMON},

    {{{1, 1, 1, 1}},
     4,
     1,
     4,
     31,
     COMMON},

    {{{1, 0, 0},
      {1, 1, 1}},
     3,
     2,
     4,
     31,
     COMMON},

    {{{0, 1},
      {0, 1},
      {1, 1},
      {1, 0}},
     2,
     4,
     5,
     31,
     COMMON},

    {{{1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}},
     3,
     3,
     8,
     31,
     RARE},

    {{{0, 1, 1},
      {1, 1, 0}},
     3,
     2,
     4,
     31,
     COMMON},

    {{{1, 0},
      {1, 1},
      {1, 0}},
     2,
     3,
     4,
     31,
     COMMON},

    {{{1, 0, 1},
      {1, 1, 1}},
     3,
     2,
     5,
     31,
     COMMON},

    {{{0, 0, 1},
      {0, 1, 1},
      {1, 1, 0}},
     3,
     3,
     5,
     31,
     COMMON},

    {{{0, 1, 0},
      {1, 1, 1},
      {0, 1, 0}},
     3,
     3,
     5,
     31,
     COMMON},

    {{{1, 1, 0},
      {0, 1, 1}},
     3,
     2,
     4,
     31,
     COMMON},

    {{{0, 1, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 1, 0},
      {1, 0, 0}},
     3,
     5,
     8,
     31,
     RARE}
};

int main(int argc, char **argv)
{
    // TODO: Print this in the window before starting the game
    printColored(1, "Welcome to Octis!\n");
    printf(" _____     _   _     \n");
    printf("|     |___| |_|_|___ \n");
    printf("|  |  |  _|  _| |_ -|\n");
    printf("|_____|___|_| |_|___|\n");
    printf("This game is a 2-player Tetris game.\n");
    printf("In order to win, the oponent has to place a block over the limit.\n");
    
    srand((int)time(NULL));
    init();
    startGame();
    finish();

    return 0;
}

void startGame()
{
    bool firstPlayerToPlay = false;
    BLOCK *firstPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    if (firstPlayerBlocks == NULL)
    {
        puts("Couldn't allocate memory to the first player blocks");
        return;
    }
    int firstPlayerBlocksAmount = MAX_BLOCK_COUNT;
    pickBlocks(firstPlayerBlocks, MAX_BLOCK_COUNT);
    BLOCK *secondPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
    if (secondPlayerBlocks == NULL)
    {
        puts("Couldn't allocate memory to the second player blocks");
        free(firstPlayerBlocks);
        return;
    }
    int secondPlayerBlocksAmount = MAX_BLOCK_COUNT;
    pickBlocks(secondPlayerBlocks, MAX_BLOCK_COUNT);
    int board[HEIGHT][WIDTH];
    createBoard(board, WIDTH, HEIGHT, OFF_VALUE);

    // Game loop
    bool gameRunning = true;
    while (gameRunning)
    {
        // Process through each event
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    int keypressed = event.key.keysym.sym;
                    if (keypressed == QUIT_KEY)
                    {
                        gameRunning = 0;
                        break;
                    }
                    break;

                case SDL_QUIT:
                    gameRunning = 0;
                    break;

                case SDL_KEYUP:
                    break;
            }
        }
    }

    /*do
    {
        firstPlayerToPlay = !firstPlayerToPlay;
        printf("Player %d to play!\n", !firstPlayerToPlay + 1);

        // Check if block list is empty for both players
        if (firstPlayerBlocksAmount <= 0)
        {
            free(firstPlayerBlocks);
            firstPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
            if (firstPlayerBlocks == NULL)
            {
                puts("Couldn't allocate memory to first player blocks");
                free(secondPlayerBlocks);
                return;
            }
            pickBlocks(firstPlayerBlocks, MAX_BLOCK_COUNT);
            firstPlayerBlocksAmount = MAX_BLOCK_COUNT;
        }
        if (secondPlayerBlocksAmount <= 0)
        {
            free(secondPlayerBlocks);
            secondPlayerBlocks = (BLOCK *)malloc(sizeof(BLOCK) * MAX_BLOCK_COUNT);
            if (secondPlayerBlocks == NULL)
            {
                puts("Couldn't allocate memory to second player blocks");
                free(firstPlayerBlocks);
                return;
            }
            pickBlocks(secondPlayerBlocks, MAX_BLOCK_COUNT);
            secondPlayerBlocksAmount = MAX_BLOCK_COUNT;
        }

        if (firstPlayerToPlay)
            firstPlayerBlocks = turn(board, firstPlayerBlocks, &firstPlayerBlocksAmount);
        else
            secondPlayerBlocks = turn(board, secondPlayerBlocks, &secondPlayerBlocksAmount);
    } while (firstPlayerBlocks != NULL && secondPlayerBlocks != NULL);*/

    printf("\nPlayer %d lost!\n", !firstPlayerToPlay + 1);

    free(firstPlayerBlocks);
    free(secondPlayerBlocks);
}

void createBoard(int board[][WIDTH], int width, int height, int baseValue)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            *(*(board + y) + x) = baseValue;
        }
    }
}

void pickBlocks(BLOCK *blocks, int blockCount)
{
    // TODO: Apply Rarity
    for (int i = 0; i < blockCount; i++)
    {
        int randomValue = rand();
        int blockIndex = randomValue % BLOCK_COUNT;
        bool doFlip = randomValue % 2 == 0;
        int rotationCount = randomValue % 3;
        int color = 31 + randomValue % 5;

        memcpy (&blocks[i], &BLOCKS[blockIndex], sizeof(BLOCK));
        if (doFlip)
            flip(&blocks[i]);
        for (int r = 0; r < rotationCount; r++)
            rotate(&blocks[i]);
        blocks[i].color = color;
    }
}

/// @brief Gets the input from the player to get his moves
/// @param board The current board
/// @param playerBlocks The player's blocks
/// @return NULL if the player lost, a list of the still available blocks otherwise
BLOCK *turn(int board[][WIDTH], BLOCK *playerBlocks, int *blocksAmount)
{
    // Print the available blocks
    printBlocks(playerBlocks, *blocksAmount);

    // Get the chosen block from the player
    char *prompt = (char *)malloc(sizeof(char) * (26 + (int)(*blocksAmount / 10)));
    sprintf(prompt, "Choose your block [1-%d]: ", *blocksAmount);
    int blockIndex = readIntFromUser(prompt, 1, *blocksAmount);
    BLOCK block = playerBlocks[blockIndex - 1];

    // Print the chosen block above the board
    BLOCK *showedBlocks = (BLOCK *)malloc(sizeof(BLOCK) * 4);
    showedBlocks[0] = block;
    unsigned short rotatedBlocksCount = 1;
    for (int i = 0; i < 3; i++)
    {
        BLOCK *rotatedBlock = (BLOCK *)malloc(sizeof(BLOCK));
        copyBlock(rotatedBlock, &showedBlocks[rotatedBlocksCount - 1]);
        rotate(rotatedBlock);
        if (compareBlock(rotatedBlock, &block))
        {
            free(rotatedBlock);
            if (i == 0)
            {
                break;
            }
            continue;
        }
        rotatedBlocksCount++;
        showedBlocks[rotatedBlocksCount - 1] = *rotatedBlock;
        free(rotatedBlock);
    }

    // Get the chosen orientation from the player
    if (rotatedBlocksCount > 1)
    {
        printBlocks(showedBlocks, rotatedBlocksCount);
        free(prompt);
        prompt = (char *)malloc(sizeof(char) * 36);
        sprintf(prompt, "Choose your orientation [1-%d]: ", rotatedBlocksCount);
        int chosenRotation = readIntFromUser(prompt, 1, rotatedBlocksCount);
        block = showedBlocks[chosenRotation - 1];
    }
    free(showedBlocks);

    // Print the rotated block above the board
    int middleX = WIDTH - block.width;
    printBlockOffset(block, middleX);
    printBoard(board);

    // Get the chosen abscissa
    int maxX = WIDTH - block.width + 1;
    free(prompt);
    prompt = (char *)malloc(sizeof(char) * (29 + (int)(maxX / 10)));
    sprintf(prompt, "Choose your abscissa [1-%d]: ", maxX);
    int chosenX = readIntFromUser(prompt, 1, maxX);
    free(prompt);

    // Place the block in the board
    if (!fall(board, &block, chosenX - 1))
    {
        free(playerBlocks);
        return NULL;
    }

    // Remove completed lines
    removeCompletedLines(board);

    // Print the board
    printBoard(board);

    // Remove the block from the list
    (*blocksAmount)--;
    BLOCK *newBlocks = (BLOCK *)malloc(sizeof(BLOCK) * (*blocksAmount));
    int count = 0;
    for (int i = 0; i < (*blocksAmount) + 1; i++)
    {
        if (i != blockIndex - 1)
        {
            newBlocks[count++] = playerBlocks[i];
        }
    }

    free(playerBlocks);

    return newBlocks;
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
    BLOCK *tempBlock = (BLOCK *)malloc(sizeof(BLOCK));
    copyBlock(tempBlock, block);
    block->width = tempBlock->height;
    block->height = tempBlock->width;
    for (int y = 0; y < MAX_BLOCK_SIZE; y++)
    {
        for (int x = 0; x < MAX_BLOCK_SIZE; x++)
        {
            block->block[y][x] = false;
        }
    }

    for (int y = 0; y < tempBlock->height; y++)
    {
        for (int x = 0; x < tempBlock->width; x++)
        {
            block->block[x][block->width - y - 1] = tempBlock->block[y][x];
        }
    }

    free(tempBlock);
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
    for (int y = 0; y < MAX_BLOCK_SIZE; y++)
    {
        for (int x = 0; x < MAX_BLOCK_SIZE; x++)
        {
            if (blockA->block[y][x] != blockB->block[y][x])
                return false;
        }
    }
    return true;
}

void copyBlock(BLOCK *blockA, BLOCK *blockB)
{
    for (int y = 0; y < MAX_BLOCK_SIZE; y++)
    {
        for (int x = 0; x < MAX_BLOCK_SIZE; x++)
        {
            blockA->block[y][x] = blockB->block[y][x];
        }
    }
    blockA->width = blockB->width;
    blockA->height = blockB->height;
    blockA->count = blockB->count;
    blockA->color = blockB->color;
    // Not really necessary since it's not used after block choosing
    // blockA->chance = blockB->chance;
}

void copyBoard(int boardA[][WIDTH], int boardB[][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            *(*(boardA + y) + x) = *(*(boardB + y) + x);
        }
    }
}

int countBoardSquares(int board[][WIDTH], int offValue)
{
    int count = 0;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (*(*(board + y) + x) != offValue)
                count++;
        }
    }
    return count;
}

void addBlock(int board[][WIDTH], BLOCK *block, int X, int Y)
{
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            if (block->block[y][x])
            {
                *(*(board + y + Y) + x + X) = block->color;
            }
        }
    }
}

bool fall(int board[][WIDTH], BLOCK *block, int X)
{
    int totalSquares = block->count + countBoardSquares(board, OFF_VALUE);
    int height = 0;
    while (height + block->height <= HEIGHT)
    {
        int boardCopy[HEIGHT][WIDTH];
        copyBoard(boardCopy, board);
        // For the first iteration, we consider the block as a full rectangle to avoid clipping
        int addedSquares = 0;
        if (height == 0)
        {
            BLOCK* fullRectangle = (BLOCK *)malloc(sizeof(BLOCK));
            copyBlock(fullRectangle, block);
            for (int x = 0; x < block->width; x++)
            {
                bool blocksStarted = false;
                for (int y = block->height - 1; y >= 0; y--)
                {
                    if (!blocksStarted && fullRectangle->block[y][x])
                    {
                        blocksStarted = true;
                    }
                    if (blocksStarted && !fullRectangle->block[y][x])
                    {
                        fullRectangle->block[y][x] = true;
                        addedSquares++;
                    }
                }
            }
            addBlock(boardCopy, fullRectangle, X, height);
            free(fullRectangle);
        }
        addBlock(boardCopy, block, X, height);
        totalSquares += addedSquares;
        if (totalSquares != countBoardSquares(boardCopy, OFF_VALUE))
        {
            if (height - 1 < 0)
            {
                return false;
            }
            break;
        }
        totalSquares -= addedSquares;
        height++;
    }
    addBlock(board, block, X, height - 1);
    return true;
}

bool isLineFinished(int *line, int offValue)
{
    for (int x = 0; x < WIDTH; x++)
    {
        if (*(line + x) == offValue)
        {
            return false;
        }
    }
    return true;
}

void removeCompletedLines(int board[][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        if (isLineFinished(*(board + y), OFF_VALUE))
        {
            for (int i = y; i > 0; i--)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    if (i == 0)
                    {
                        *(*(board) + x) = OFF_VALUE;
                        continue;
                    }
                    *(*(board + i) + x) = *(*(board + i - 1) + x);
                }
            }
        }
    }
}

void printBlocks(BLOCK *blocks, int blocksAmount)
{
    char spacing[6] = "     ";
    for (int y = 0; y < MAX_BLOCK_SIZE; y++)
    {
        for (int i = 0; i < blocksAmount; i++)
        {
            for (int x = 0; x < blocks[i].width; x++)
            {
                if (y < blocks[i].height && blocks[i].block[y][x])
                {
                    printColored(blocks[i].color, "■ ");
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

void printBlock(BLOCK block)
{
    printBlockOffset(block, 0);
}

void printBlockOffset(BLOCK block, int offset)
{
    for (int y = 0; y < block.height; y++)
    {
        for (int i = 0; i < offset; i++)
        {
            printf(" ");
        }
        for (int x = 0; x < block.width; x++)
        {
            if (block.block[y][x])
            {
                printColored(block.color, "■ ");
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
}

char *multiplyChar(char character, int amount)
{
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

void printBoard(int board[][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (*(*(board + y) + x) != OFF_VALUE)
                printColored(*(*(board + y) + x), "■ ");
            else
                printf("  ");
        }
        printf("%d\n", HEIGHT - y);
    }

    for (int x = 1; x <= WIDTH; x++)
    {
        printf("%d ", x);
    }
    printf("\n");
}
