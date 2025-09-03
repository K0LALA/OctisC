#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "screen.h"
#include "octis.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Color foregroundColor = {255, 255, 255, 255};

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

int BLOCK_SIZE;
int BLOCK_BORDER_SIZE;

int BLOCKS_WIDTH;
int BLOCKS_HEIGHT;

int BLOCKS_X;
int BLOCKS_Y;

int BOARD_H;
int BOARD_BLOCK_SIZE;

int BOARD_X;
int BOARD_Y;

typedef struct
{
    BLOCK **firstPlayerBlocks;
    int *firstPlayerAmount;
    BLOCK **secondPlayerBlocks;
    int *secondPlayerAmount;
    bool *firstPlayerToPlay;
    int *blockIndex;
} REDRAW_POINTERS;

REDRAW_POINTERS redrawPointers;

// This texture is used as an intermediate to render the screen
// It's possible to change only portions of it so a render of everything isn't necessary
SDL_Texture *mainTexture;

// Follows the same order as ANSI escape codes for colors (30-37):
static const SDL_Color COLORS[] =
    {
        {134, 134, 141, 255}, // Black
        {243, 139, 168, 255}, // Red
        {166, 227, 161, 255}, // Green
        {249, 226, 175, 255}, // Yellow
        {137, 180, 250, 255}, // Blue
        {203, 166, 247, 255}, // Magenta
        {137, 220, 235, 255}, // Cyan
        {205, 214, 244, 255}  // White
};

void calculateSizes()
{
    if (window != NULL)
    {
        SDL_GetWindowSizeInPixels(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    }

    BLOCKS_WIDTH = SCREEN_WIDTH / 5;
    BLOCKS_HEIGHT = SCREEN_HEIGHT - 60;

    BLOCK_SIZE = (BLOCKS_HEIGHT - BLOCKS_MARGIN) / (MAX_BLOCK_SIZE * MAX_BLOCK_COUNT);
    BLOCK_BORDER_SIZE = BLOCK_SIZE / 5;

    BLOCKS_X = 50;
    BLOCKS_Y = 30;

    BOARD_H = SCREEN_HEIGHT - 300;
    BOARD_BLOCK_SIZE = BOARD_H / HEIGHT;

    BOARD_X = (SCREEN_WIDTH - BOARD_BLOCK_SIZE * WIDTH) / 2;
    BOARD_Y = 100;

    mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() < 0)
    {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    calculateSizes();

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS, &window, &renderer) != 0)
    {
        printf("SDL_CreateWindowAndRenderer Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowMinimumSize(window, 1024, 768);

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowTitle(window, "Octis");

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!mainTexture)
    {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void updateRedrawPointers(BLOCK **p1Blocks, int *p1Amount, BLOCK **p2Blocks, int *p2Amount, bool *p1Play)
{
    REDRAW_POINTERS temp = {p1Blocks, p1Amount, p2Blocks, p2Amount, p1Play, NULL};
    redrawPointers = temp;
}

void updateRedrawBlockIndex(int *blockIndex)
{
    redrawPointers.blockIndex = blockIndex;
}

void finish()
{
    SDL_DestroyTexture(mainTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

/// @brief Handles common events
/// @param event The event after calling SDL_PollEvent()
/// @return 0 if nothing is to be done, 1 for quit, 2 for redraw
int handleEvents(SDL_Event event)
{
    // Quit signals
    if (event.type == SDL_QUIT ||
        (event.type == SDL_KEYDOWN && event.key.keysym.sym == QUIT_KEY))
    {
        return 1;
    }
    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            calculateSizes();
            return 2;
        }
        if (event.window.event == SDL_WINDOWEVENT_SHOWN)
        {
            return 2;
        }
    }
    return 0;
}

void updateScreen()
{
    SDL_RenderPresent(renderer);
}

void clearScreen()
{
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void renderOnMainTexture()
{
    if (SDL_SetRenderTarget(renderer, mainTexture) < 0)
    {
        printf("SDL_SetRenderTarget Error: %s\n", SDL_GetError());
    }
}

void renderPresentFromTexture()
{
    // Set render target to screen
    if (SDL_SetRenderTarget(renderer, NULL) < 0)
    {
        printf("SDL_SetRenderTarget2 Error: %s\n", SDL_GetError());
        return;
    }
    // Render copy from texture to screen
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL);

    // Render present
    SDL_RenderPresent(renderer);
}

int getWindowWidth()
{
    return SCREEN_WIDTH;
}

int getWindowHeight()
{
    return SCREEN_HEIGHT;
}

void redraw(int board[][WIDTH])
{
    renderScreen(board, *redrawPointers.firstPlayerBlocks, *redrawPointers.firstPlayerAmount, *redrawPointers.secondPlayerBlocks, *redrawPointers.secondPlayerAmount, *redrawPointers.firstPlayerToPlay);
}

/// @brief Renders everything on the screen, SDL_RenderPresent() is ran at the end
void renderScreen(int board[][WIDTH], BLOCK *firstBlocks, int firstBlocksAmount, BLOCK *secondBlocks, int secondBlocksAmount, bool firstPlayerToPlay)
{
    // Clear the screen
    // Set render target to a texture
    if (SDL_SetRenderTarget(renderer, mainTexture) < 0)
    {
        printf("SDL_SetRenderTarget Error: %s\n", SDL_GetError());
        return;
    }
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // Apply all changes
    renderBoard(board);
    int index = -1;
    if (redrawPointers.blockIndex != NULL)
    {
        index = *redrawPointers.blockIndex;
    }
    renderBlocksSelection(firstBlocks, firstBlocksAmount, firstPlayerToPlay ? index : -1, true);
    renderBlocksSelection(secondBlocks, secondBlocksAmount, firstPlayerToPlay ? -1 : index, false);
    char playerText[19];
    sprintf(playerText, "Player %d to play!", !firstPlayerToPlay + 1);
    renderText(playerText, foregroundColor, -1, SCREEN_HEIGHT - 100, 32);
    // Set render target to screen
    if (SDL_SetRenderTarget(renderer, NULL) < 0)
    {
        printf("SDL_SetRenderTarget2 Error: %s\n", SDL_GetError());
        return;
    }
    // Render copy from texture to screen
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL);

    // Render present
    SDL_RenderPresent(renderer);
}

/// @brief Renders text on the window, SDl_RenderPresent needs to be called to view changes
/// @param text The text to display
/// @param color The foreground color for the text
/// @param x The abscissa for the text, -1 for centered
/// @param y The ordinate for the text, -1 for centered
/// @param fontSize The size size of the font
/// @return 0 for success, 1 otherwise
int renderText(const char *text, SDL_Color color, int x, int y, int fontSize)
{
    TTF_Font *font = TTF_OpenFont(FONT_NOTO_SANS, fontSize);
    if (!font)
    {
        printf("TTF_OpenFont Error: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, color);
    if (!textSurface)
    {
        printf("TTF_RenderText_Solid Error: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture)
    {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    // Center if needed
    if (x == -1)
    {
        x = (SCREEN_WIDTH - textSurface->w) / 2;
    }
    if (y == -1)
    {
        y = (SCREEN_HEIGHT - textSurface->h) / 2;
    }

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    if (SDL_RenderCopy(renderer, textTexture, NULL, &textRect) < 0)
    {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return 1;
    }

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    return 0;
}

static void renderCell(SDL_Color color, const int x, const int y, int blockSize)
{
    SDL_Rect borderRect, insideRect;
    borderRect.x = x;
    borderRect.y = y;
    borderRect.w = blockSize;
    borderRect.h = blockSize;
    insideRect.x = borderRect.x + blockSize / 5;
    insideRect.y = borderRect.y + blockSize / 5;
    insideRect.w = blockSize - blockSize / 5 * 2;
    insideRect.h = blockSize - blockSize / 5 * 2;
    Uint8 r = color.r;
    Uint8 g = color.g;
    Uint8 b = color.b;
    if (r - 20 >= 0)
        r -= 20;
    if (g - 20 >= 0)
        g -= 20;
    if (b - 20 >= 0)
        b -= 20;
    SDL_SetRenderDrawColor(renderer, r, g, b, color.a);
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &insideRect);
}

static void renderBlock(BLOCK block, SDL_Color color, const int x, const int y)
{
    for (int blockY = 0; blockY < block.height; blockY++)
    {
        for (int blockX = 0; blockX < block.width; blockX++)
        {
            if (block.block[blockY][blockX])
            {
                renderCell(color, x + blockX * BLOCK_SIZE, y + blockY * BLOCK_SIZE, BLOCK_SIZE);
            }
        }
    }
}

void renderBoard(int board[][WIDTH])
{
    int x = BOARD_X;
    int y = BOARD_Y;
    SDL_Rect borderRect, insideRect;
    borderRect.x = x;
    borderRect.y = y;
    borderRect.w = 2 * BORDER_SIZE + WIDTH * BOARD_BLOCK_SIZE;
    borderRect.h = 2 * BORDER_SIZE + HEIGHT * BOARD_BLOCK_SIZE;
    x += BORDER_SIZE;
    y += BORDER_SIZE;
    insideRect.x = x;
    insideRect.y = y;
    insideRect.w = WIDTH * BOARD_BLOCK_SIZE;
    insideRect.h = HEIGHT * BOARD_BLOCK_SIZE;
    Uint8 r = 12;
    Uint8 g = 7;
    Uint8 b = 18;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, r + 10, g + 10, b + 10, 255);
    SDL_RenderFillRect(renderer, &insideRect);
    for (int boardY = 0; boardY < HEIGHT; boardY++)
    {
        for (int boardX = 0; boardX < WIDTH; boardX++)
        {
            if (board[boardY][boardX] != OFF_VALUE)
            {
                renderCell(COLORS[board[boardY][boardX] - 30], x + boardX * BOARD_BLOCK_SIZE, y + boardY * BOARD_BLOCK_SIZE, BOARD_BLOCK_SIZE);
            }
        }
    }
}

// TODO: Allow for more blocks in hand, hide some while keeping selected in middle of selection, still visible
void renderBlocksSelection(BLOCK *blocks, int blocksAmount, int blockIndex, bool firstPlayerToPlay)
{
    int x = firstPlayerToPlay ? BLOCKS_X : SCREEN_WIDTH - BLOCKS_WIDTH - BLOCKS_X;
    int y = BLOCKS_Y;
    // These rectangles also clear the area
    SDL_Rect borderRect, insideRect;
    borderRect.x = x;
    borderRect.y = y;
    borderRect.w = 2 * BORDER_SIZE + BLOCKS_WIDTH;
    borderRect.h = 2 * BORDER_SIZE + BLOCKS_HEIGHT;
    x += BORDER_SIZE;
    y += BORDER_SIZE;
    insideRect.x = x;
    insideRect.y = y;
    insideRect.w = BLOCKS_WIDTH;
    insideRect.h = BLOCKS_HEIGHT;
    Uint8 r = 12;
    Uint8 g = 7;
    Uint8 b = 18;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, r + 10, g + 10, b + 10, 255);
    SDL_RenderFillRect(renderer, &insideRect);
    int verticalSpacing = BLOCKS_HEIGHT;
    for (int i = 0; i < blocksAmount; i++)
    {
        verticalSpacing -= blocks[i].height * BLOCK_SIZE;
    }
    verticalSpacing /= blocksAmount + 1;
    int offsetY = verticalSpacing;
    for (int i = 0; i < blocksAmount; i++)
    {
        int offsetX = (BLOCKS_WIDTH - BLOCK_SIZE * blocks[i].width) / 2;
        SDL_Color blockColor = COLORS[blocks[i].color - 30];
        if (blockIndex == -1 || i != blockIndex)
        {
            int gray = blockColor.r + blockColor.g + blockColor.b;
            gray /= 3;
            blockColor.r = gray;
            blockColor.g = gray;
            blockColor.b = gray;
        }
        renderBlock(blocks[i], blockColor, x + offsetX, y + offsetY);
        offsetY += verticalSpacing + blocks[i].height * BLOCK_SIZE;
    }
}

void renderBlocks(BLOCK *blocks, int blocksAmount, bool firstPlayerToPlay)
{
    renderBlocksSelection(blocks, blocksAmount, -1, firstPlayerToPlay);
}
