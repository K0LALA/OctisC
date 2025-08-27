#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "screen.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer;

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
    if (y == - 1)
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
