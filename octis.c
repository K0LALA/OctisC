#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "octis.h"
#include "screen.h"

SDL_Color pureWhite = {255, 255, 255, 255};

SDL_Event event;
int keypressed;
int buttonpressed;

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
     RARE}};

int main(int argc, char **argv)
{
    printColored(31, "Welcome to Octis!\n");
    printf(" _____     _   _     \n");
    printf("|     |___| |_|_|___ \n");
    printf("|  |  |  _|  _| |_ -|\n");
    printf("|_____|___|_| |_|___|\n");
    
    srand((int)time(NULL));
    init();
    
    renderText("Octis", pureWhite, -1, 25, 64);
    renderText("This game is a 2-player Tetris game.", pureWhite, -1, 350, 32);
    renderText("To win, make the opponent place a block above the limit!", pureWhite, -1, 400, 32);
    renderText("Press any key to start.", pureWhite, -1, 850, 48);
    updateScreen();

    // Wait for ENTER
    bool gameStarted = false;
    while (!gameStarted)
    {
        // while loop to remove all pending events which could cause issues in the next step
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYUP:
                keypressed = event.key.keysym.sym;
                if (keypressed == QUIT_KEY)
                {
                    finish();
                    return EXIT_SUCCESS;
                }
                gameStarted = true;
                break;

            case SDL_MOUSEBUTTONUP:
                gameStarted = true;
                break;

            case SDL_QUIT:
                finish();
                return EXIT_SUCCESS;
            }
        }
    }
    clearScreen();
    startGame();

    SDL_Delay(1000);

    finish();

    return EXIT_SUCCESS;
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

    do
    {
        firstPlayerToPlay = !firstPlayerToPlay;

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

        renderScreen(board, firstPlayerBlocks, firstPlayerBlocksAmount, secondPlayerBlocks, secondPlayerBlocksAmount, firstPlayerToPlay);
        if (firstPlayerToPlay)
            firstPlayerBlocks = turn(board, firstPlayerBlocks, &firstPlayerBlocksAmount, firstPlayerToPlay);
        else
            secondPlayerBlocks = turn(board, secondPlayerBlocks, &secondPlayerBlocksAmount, firstPlayerToPlay);
    } while (firstPlayerBlocks != NULL && secondPlayerBlocks != NULL);

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
    for (int i = 0; i < blockCount; i++)
    {
        int randomValue = rand();
        int blockChooser = randomValue % CHANCES_TOTAL;
        short blockIndex = 0;
        while (blockChooser > 0)
        {
            blockChooser -= BLOCKS[blockIndex].chance;
            blockIndex++;
        }
        // Shouldn't happen, just in case
        if (blockIndex > BLOCK_COUNT)
        {
            printf("blockIndex over limit: %d\n", blockIndex);
            blockIndex = BLOCK_COUNT;
        }
        bool doFlip = randomValue % 2 == 0;
        int color = 31 + randomValue % 5;

        memcpy(&blocks[i], &BLOCKS[blockIndex - 1], sizeof(BLOCK));
        if (doFlip)
            flip(&blocks[i]);
        blocks[i].color = color;
    }
}

/// @brief Make the player play his turn
/// @param board The current board
/// @param playerBlocks The player's blocks
/// @param firstPlayerToPlay Whose turn is it
/// @return NULL if the player lost, a list of the still available blocks otherwise
BLOCK *turn(int board[][WIDTH], BLOCK *playerBlocks, int *blocksAmount, bool firstPlayerToPlay)
{
    // Get the chosen block from the player
    int blockIndex = 0;
    bool isBlockSelected = false;
    bool didSelectedBlockChanged = true;
    while (!isBlockSelected)
    {
        if (didSelectedBlockChanged)
        {
            // Display the blocks with only the selected one colored
            renderOnMainTexture();
            renderBlocksSelection(playerBlocks, *blocksAmount, blockIndex, firstPlayerToPlay ? BLOCKS_X : SCREEN_WIDTH - BLOCKS_WIDTH - BLOCKS_X, BLOCKS_Y);
            renderPresentFromTexture();
            didSelectedBlockChanged = false;
        }
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                keypressed = event.key.keysym.sym;
                switch(keypressed)
                {
                case QUIT_KEY:
                    // TODO: Handle this better since NULL already means LOSS
                    return NULL;
                    break;

                case SDLK_RETURN:
                case SDLK_RETURN2:
                    isBlockSelected = true;
                    break;

                case SDLK_LEFT:
                case SDLK_UP:
                    blockIndex--;
                    if (blockIndex < 0) blockIndex = *blocksAmount - 1;
                    didSelectedBlockChanged = true;
                    break;

                case SDLK_RIGHT:
                case SDLK_DOWN:
                    blockIndex++;
                    if (blockIndex >= *blocksAmount) blockIndex = 0;
                    didSelectedBlockChanged = true;
                    break;
                }
                break;
            
            case SDL_QUIT:
                return NULL;
                break;
            }
        }
    }
    BLOCK block = playerBlocks[blockIndex];
    BLOCK tmpBlock;
    copyBlock(&tmpBlock, &block);

    // Remove the block from the list
    (*blocksAmount)--;
    BLOCK *newBlocks = (BLOCK *)malloc(sizeof(BLOCK) * (*blocksAmount));
    int count = 0;
    for (int i = 0; i < (*blocksAmount) + 1; i++)
    {
        if (i != blockIndex)
        {
            newBlocks[count++] = playerBlocks[i];
        }
    }

    free(playerBlocks);

    renderOnMainTexture();
    renderBlocks(newBlocks, *blocksAmount, firstPlayerToPlay ? BLOCKS_X : SCREEN_WIDTH - BLOCKS_WIDTH - BLOCKS_X, BLOCKS_Y);
    renderPresentFromTexture();

    int currentX = (WIDTH - block.width) / 2;
    int targetX = currentX;
    int maxX = WIDTH - block.width;

    int expectedSquares = block.count + countBoardSquares(board, OFF_VALUE);
    int height = 0;
    bool fallEnded = false;
    bool firstPlace = true;
    int maxHeight = HEIGHT - block.height;

    int lastIncrement = (int) time(NULL);

    bool needsRender = true;
    do {
        while (!fallEnded && SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                keypressed = event.key.keysym.sym;
                switch(keypressed)
                {
                case QUIT_KEY:
                    free(newBlocks);
                    return NULL;
                    break;

                case SDLK_LEFT:
                    targetX--;
                    if (targetX < 0) targetX = 0;
                    needsRender = true;
                    break;

                case SDLK_RIGHT:
                    targetX++;
                    if (targetX > maxX) targetX = maxX;
                    needsRender = true;
                    break;

                case SDLK_UP:
                    maxHeight = HEIGHT - block.width;
                    maxX = WIDTH - block.height;
                    // To rotate a block around its center point, we need to center the old height in the old width (X) and the old width in the old height ()
                    // These values an be negative
                    int centerOffsetX = (block.width - block.height) / 2;
                    int centerOffsetY = (block.height - block.width) / 2;
                    // Calculate new x and y if they fit on the board
                    if (targetX + centerOffsetX > maxX || height + centerOffsetY > maxHeight)
                    {
                        maxHeight = HEIGHT - block.height;
                        maxX = WIDTH - block.width;
                        break;
                    }
                    targetX += centerOffsetX;
                    height += centerOffsetY;

                    rotate(&tmpBlock);
                    needsRender = true;
                    break;

                case SDLK_DOWN:
                    height++;
                    if (height > maxHeight) continue;
                    needsRender = true;
                    break;
                }
                break;

            case SDL_QUIT:
                free(newBlocks);
                return NULL;
                break;
            }
        }

        if (needsRender)
        {
            needsRender = false;

            // Check if the move is possible
            int boardCopy[HEIGHT][WIDTH];
            copyBoard(boardCopy, board);
            int addedSquares = 0;
            if (firstPlace)
            {
                BLOCK fullRectangle;
                copyBlock(&fullRectangle, &tmpBlock);
                for (int x = 0; x < block.width; x++)
                {
                    bool blocksStarted = false;
                    for (int y = block.height - 1; y >= 0; y--)
                    {
                        if (!blocksStarted && fullRectangle.block[y][x])
                        {
                            blocksStarted = true;
                        }
                        if (blocksStarted && !fullRectangle.block[y][x])
                        {
                            fullRectangle.block[y][x] = true;
                            addedSquares++;
                        }
                    }
                }
                addBlock(boardCopy, &fullRectangle, targetX, height);
            }
            else {
                addBlock(boardCopy, &tmpBlock, targetX, height);
            }
            expectedSquares += addedSquares;
            // Illegal move
            if (expectedSquares != countBoardSquares(boardCopy, OFF_VALUE))
            {
                // First placement failed
                if (firstPlace)
                {
                    copyBoard(boardCopy, board);
                    addBlock(boardCopy, &block, currentX, height);
                    renderOnMainTexture();
                    renderBoard(boardCopy, BOARD_X, BOARD_Y);
                    renderPresentFromTexture();
                    free(newBlocks);
                    return NULL;
                }
                // Sideway move failed
                if (targetX != currentX)
                {
                    targetX = currentX;
                }
                // Rotation failed
                else if (!compareBlock(&tmpBlock, &block))
                {
                    copyBlock(&tmpBlock, &block);
                    maxHeight = HEIGHT - block.height;
                }
                // Reached block at the bottom
                else {
                    fallEnded = true;
                    break;
                }
            } else {
                // Legal move
                if (firstPlace)
                {
                    // Render using the original block, not the rectangle
                    copyBoard(boardCopy, board);
                    addBlock(boardCopy, &block, currentX, height);
                    firstPlace = false;
                }

                currentX = targetX;
                copyBlock(&block, &tmpBlock);
                
                // Render the board
                renderOnMainTexture();
                renderBoard(boardCopy, BOARD_X, BOARD_Y);
                renderPresentFromTexture();

                // Reset the timer for height increments to avoid strange results
                lastIncrement = (int) time(NULL);
            }
            expectedSquares -= addedSquares;                
        }

        // Increment height periodically
        if ((int) time(NULL) - lastIncrement >= 1)
        {
            height++;
            lastIncrement = (int) time(NULL);
            needsRender = true;
        }
    } while (height <= maxHeight && !fallEnded);
    addBlock(board, &block, currentX, height - 1);

    // Remove completed lines
    removeCompletedLines(board);

    return newBlocks;
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

/// @brief Compares data of blockA and blockB
/// @param blockA a block
/// @param blockB a second block
/// @return true if blockA and blockB are the same, false otherwise
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

/// @brief Copies blockB data in blockA data
/// @param blockA destination
/// @param blockB source
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

/// @brief Copies boardB data in boardA data
/// @param boardA destination
/// @param boardB source
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
            if (y + Y <= HEIGHT && x + X <= WIDTH && block->block[y][x])
            {
                *(*(board + y + Y) + x + X) = block->color;
            }
        }
    }
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

void printColored(int color, const char *text)
{
    printf("\033[%dm%s\033[0m", color, text);
}
