#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "screen.h"
#include "octis.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Color foregroundColor = {255, 255, 255, 255};

static const SDL_Color COLORS[] = {
    {50, 50, 50, 255},         // Black
    {255, 0, 0, 255},       // Red
    {0, 255, 0, 255},       // Green
    {255, 255, 0, 255},     // Yellow
    {0, 0, 255, 255},       // Blue
    {255, 0, 255, 255},     // Magenta
    {0, 255, 255, 255},     // Cyan
    {255, 255, 255, 255}    // White
};

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

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0)
    {
        printf("SDL_CreateWindowAndRenderer Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowTitle(window, "Octis");

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void finish()
{
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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

void renderScreen(int board[][WIDTH], BLOCK *firstBlocks, int firstBlocksAmount, BLOCK *secondBlocks, int secondBlocksAmount, bool firstPlayerToPlay)
{
    renderBoard(board, (SCREEN_WIDTH - WIDTH * BLOCK_SIZE) / 2, 50);
    renderBlocks(firstBlocks, firstBlocksAmount, 50, 30);
    renderBlocks(secondBlocks, secondBlocksAmount, SCREEN_WIDTH - BLOCKS_WIDTH - 50, 30);
    char playerText[19];
    sprintf(playerText, "Player %d to play!", !firstPlayerToPlay + 1);
    renderText(playerText, foregroundColor, -1, 900, 32);

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

    TTF_CloseFont(font);

    return 0;
}

static void renderCell(SDL_Color color, int x, int y)
{
    SDL_Rect borderRect, insideRect;
    borderRect.x = x;
    borderRect.y = y;
    borderRect.w = BLOCK_SIZE;
    borderRect.h = BLOCK_SIZE;
    insideRect.x = borderRect.x + BLOCK_BORDER_SIZE;
    insideRect.y = borderRect.y + BLOCK_BORDER_SIZE;
    insideRect.w = BLOCK_SIZE - BLOCK_BORDER_SIZE * 2;
    insideRect.h = BLOCK_SIZE - BLOCK_BORDER_SIZE * 2;
    Uint8 r = color.r;
    Uint8 g = color.g;
    Uint8 b = color.b;
    if (r - 20 >= 0) r -= 20;
    if (g - 20 >= 0) g -= 20;
    if (b - 20 >= 0) b -= 20;
    SDL_SetRenderDrawColor(renderer, r, g, b, color.a);
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &insideRect);
}

static void renderBlock(BLOCK block, SDL_Color color, int x, int y)
{
    for (int blockY = 0; blockY < block.height; blockY++)
    {
        for (int blockX = 0; blockX < block.width; blockX++)
        {
            if (block.block[blockY][blockX])
            {
                renderCell(color, x + blockX * BLOCK_SIZE, y + blockY * BLOCK_SIZE);
            }
        }
    }
}

void renderBoard(int board[][WIDTH], int x, int y)
{
    for (int boardY = 0; boardY < HEIGHT; boardY++)
    {
        for (int boardX = 0; boardX < WIDTH; boardX++)
        {
            if (board[boardY][boardX] != OFF_VALUE)
            {
                renderCell(COLORS[board[boardY][boardX] - 30], x + boardX * BLOCK_SIZE, y + boardY * BLOCK_SIZE);
            }
            else {
                renderCell(COLORS[0], x + boardX * BLOCK_SIZE, y + boardY * BLOCK_SIZE);
            }
        }
    }
}

void renderBlocks(BLOCK* blocks, int blocksAmount, int x, int y)
{
    int offsetY = 0;
    for (int i = 0; i < blocksAmount; i++)
    {
        renderBlock(blocks[i], COLORS[blocks[i].color - 30], x, y + offsetY);
        offsetY += BLOCKS_MARGIN + BLOCK_SIZE * blocks[i].height;
    }
}
