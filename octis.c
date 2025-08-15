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
     RARE},

    {{{1, 1, 0},
      {0, 1, 1},
      {0, 1, 0}},
     3,
     3,
     COMMON},

    {{{1, 1, 1, 1}},
     4,
     1,
     COMMON},

    {{{1, 0, 0},
      {1, 1, 1}},
     3,
     2,
     COMMON},

    {{{0, 1},
      {0, 1},
      {1, 1},
      {1, 0}},
     2,
     4,
     COMMON},

    {{{1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}},
     3,
     3,
     RARE},

    {{{0, 1, 1},
      {1, 1, 0}},
     3,
     2,
     COMMON},

    {{{1, 0},
      {1, 1},
      {1, 0}},
     2,
     3,
     COMMON},

    {{{1, 0, 1},
      {1, 1, 1}},
     3,
     2,
     COMMON},

    {{{0, 0, 1},
      {0, 1, 1},
      {1, 1, 0}},
     3,
     3,
     COMMON},

    {{{0, 1, 0},
      {1, 1, 1},
      {0, 1, 0}},
     3,
     3,
     COMMON},

    {{{1, 1, 0},
      {0, 1, 1}},
     3,
     2,
     COMMON},

    {{{0, 1, 1},
      {0, 1, 0},
      {0, 1, 1},
      {1, 1, 0},
      {1, 0, 0}},
     3,
     5,
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
    
    //printf("\033[%dm%s\033[0m\n", 32, "Press Enter to start the game");
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
    createBoard(board, WIDTH, HEIGHT, (int)' ');

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

void createBoard(int **board, const int width, const int height, const int baseValue)
{
    int *line = (int *)malloc(sizeof(int) * width);
    for (int x = 0; x < width; x++)
    {
        line[x] = baseValue;
    }
    for (int y = 0; y < height; y++)
    {
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
    // TODO: Randomly rotate and or flip for more diversity

    // For each player, pick three random numbers
    // Map each of those numbers to the range [0, BLOCK_COUNT)
    // and assign the corresponding blocks to the players
    for (int i = 0; i < blockCount; i++)
    {
        int firstPlayerIndex = rand() % BLOCK_COUNT;
        int secondPlayerIndex = rand() % BLOCK_COUNT;

        firstPlayerBlocks[i] = BLOCKS[firstPlayerIndex];
        secondPlayerBlocks[i] = BLOCKS[secondPlayerIndex];
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
    char *prompt = (char *) malloc(sizeof(char) * (25 + (int)(blocksAmount / 10)));
    sprintf(prompt, "Choose your block [1-%d]: ", blocksAmount);
    int blockIndex = readIntFromUser(prompt, 1, blocksAmount);
    BLOCK block = playerBlocks[blockIndex - 1];

    // Print the chosen block above the board
    // Or print the rotation variation which are different only
    printBlock(block);
    rotate(&block);
    printBlock(block);

    // Get the chosen orientation from the player

    // Print the rotated block above the board

    // Get the chosen abscissa

    // Place the block in the board

    // Remove completed lines

    // Print the board

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
    for (int y = 0; y < tempBlock.height; y++)
    {
        for (int x = 0; x < tempBlock.width; x++)
        {
            block->block[x][tempBlock.height - y - 1] = tempBlock.block[y][x];
        }
    }
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
    for (int y = 0; y < block.height; y++)
    {
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
}

char* multiplyChar(char character, int amount)
{
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

void printBoard(int **board, const int width, const int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            printf("%c ", (char)board[y][x]);
        }
        printf("%d\n", height - y);
    }

    for (int x = 1; x <= width; x++)
    {
        printf("%d ", x);
    }
    printf("\n");
}
