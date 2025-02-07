#include <SDL.h>
#include "clay.h"
#include <stdio.h>

typedef struct {
    uint32_t fontId;
    TTF_Font *font;
} SDL2_Font;

Clay_Dimensions SDL2_MeasureText(Clay_String *text, Clay_TextElementConfig *config);
void Clay_SDL2_Render(SDL_Renderer *renderer, Clay_RenderCommandArray renderCommands);
extern SDL2_Font SDL2_fonts[10];
