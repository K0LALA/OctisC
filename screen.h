#ifndef SCREEN_H
#define SCREEN_H

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define FONT_NOTO_SANS "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"

void init();
void finish();

void updateScreen();
int renderText(const char *text, SDL_Color color, int x, int y, int fontSize);

#endif