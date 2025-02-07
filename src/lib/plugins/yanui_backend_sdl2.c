#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "clay.h"
#include "yanui_backend_sdl2.h"


#define EXPAND_COLOR(clay_color) clay_color.r, clay_color.g, clay_color.b, clay_color.a
#define CLAY_COLOR_TO_SDL2_COLOR(clay_color) (SDL2_Color) { clay_color.r, clay_color.g, clay_color.b, clay_color.h }

const float PI = 3.1415926f;

SDL2_Font SDL2_fonts[10];


Clay_Dimensions SDL2_MeasureText(Clay_String *text, Clay_TextElementConfig *config) {
    TTF_Font *f = SDL2_fonts[config->fontId].font;
    char *chars = (char *) calloc(text->length + 1, 1);

    memcpy(chars, text->chars, text->length);
    int width = 0, height = 0;
    if (TTF_SizeUTF8(f, chars, &width, &height) < 0) {
        fprintf(stderr, "[plugin::yanui::backend] Error: could not measure text: %s\n", TTF_GetError());
        //exit(EXIT_FAILURE);
        return (Clay_Dimensions) {
            .width = 0,
            .height = 0,
        };
    } 

    free(chars);
    return (Clay_Dimensions) {
        .width = (float) width,
        .height = (float) height,
    };
}


static void SDL_DrawCircle(SDL_Renderer *renderer, SDL_FPoint center, float radius, int sd, int ed) {
    if (ed < sd) {
        int tmp = sd;
        sd = ed;
        ed = tmp;
    }
    if (ed == 0 && sd == 0) {
        ed = 360;
    }
    for (int deg = sd; deg < ed; deg++) {
        float theta = (float) deg / 180.0f * PI;
        SDL_RenderDrawPointF(renderer, center.x + SDL_cosf(theta) * radius, center.y + SDL_sinf(theta) * radius);
    }
}

void SDL_FillCircle(SDL_Renderer *renderer, SDL_FPoint center, float radius, int sd, int ed) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, center.x + dx, center.y + dy);
            }
        }
    }
    // if (ed < sd) {
    //     int tmp = sd;
    //     sd = ed;
    //     ed = tmp;
    // }
    // if (ed == 0 && sd == 0) {
    //     ed = 360;
    // }
    // for (int deg = sd; deg < ed; deg++) {
    //     float theta = (float) deg / 180.0f * PI;
    //     for (int i = 0; i < radius; i++) {
    //         SDL_RenderDrawPointF(renderer, center.x + SDL_cosf(theta) * i, center.y + SDL_sinf(theta) * i);
    //     }
    // }
}

static void SDL_FillRoundRect(SDL_Renderer *renderer, SDL_FRect rect, int radius) {
    SDL_FillCircle(renderer, (SDL_FPoint) { rect.x + radius, rect.y + radius }, radius, 0, 0);
    SDL_FillCircle(renderer, (SDL_FPoint) { rect.x + rect.w - radius, rect.y + radius }, radius, 0, 0);
    SDL_FillCircle(renderer, (SDL_FPoint) { rect.x + radius, rect.y + rect.h - radius }, radius, 0, 0);
    SDL_FillCircle(renderer, (SDL_FPoint) { rect.x + rect.w - radius, rect.y + rect.h - radius }, radius, 0, 0);

    SDL_RenderDrawLine(renderer, rect.x + radius, rect.y, rect.x + rect.w - radius, rect.y);
    SDL_RenderDrawLine(renderer, rect.x + radius, rect.y + rect.h, rect.x + rect.w - radius, rect.y + rect.h);
    SDL_RenderDrawLine(renderer, rect.x, rect.y + radius, rect.x, rect.y + rect.h - radius);
    SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y + radius, rect.x + rect.w, rect.y + rect.h - radius);

    SDL_Rect rect_ = { rect.x + radius, rect.y, rect.w - 2 * radius, rect.h };
    SDL_Rect mask_ = { rect.x, rect.y + radius, rect.w, rect.h - 2 * radius };
    SDL_RenderFillRect(renderer, &rect_);
    SDL_RenderFillRect(renderer, &mask_);
} 


SDL_Rect currentClippingRectangle;

#ifdef ENABLE_PLUGINS
void Clay_SDL2_Render(SDL_Renderer *renderer, Clay_RenderCommandArray renderCommands) {
    for (unsigned i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, i);
        Clay_BoundingBox boundingBox = renderCommand->boundingBox;
        switch (renderCommand->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            Clay_RectangleElementConfig *config = renderCommand->config.rectangleElementConfig;
            Clay_Color color = config->color;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_FRect rect = (SDL_FRect) {
                .x = boundingBox.x,
                .y = boundingBox.y,
                .w = boundingBox.width,
                .h = boundingBox.height,
            };

            if (config->cornerRadius.topLeft > 0) {
                SDL_FillRoundRect(renderer, rect, config->cornerRadius.topLeft);
            } else {
                SDL_RenderFillRectF(renderer, &rect);
            }
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            Clay_TextElementConfig *config = renderCommand->config.textElementConfig;
            Clay_String text = renderCommand->text;
            char *textCopy = (char *) calloc(text.length + 1, 1);
            memcpy(textCopy, text.chars, text.length);

            TTF_Font *font = SDL2_fonts[config->fontId].font;
            SDL_Surface *surf = TTF_RenderUTF8_Blended(font, textCopy, (SDL_Color) {
                .r = config->textColor.r,
                .g = config->textColor.g,
                .b = config->textColor.b,
                .a = config->textColor.a,
            });

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect rect = (SDL_Rect) {
                .x = boundingBox.x,
                .y = boundingBox.y,
                .w = surf->w,
                .h = surf->h,
            };
            SDL_RenderCopy(renderer, texture, NULL, &rect);

            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surf);
            free(textCopy);
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
            currentClippingRectangle = (SDL_Rect) {
                .x = (int) boundingBox.x,
                .y = (int) boundingBox.y,
                .w = (int) boundingBox.width,
                .h = (int) boundingBox.height,
            };
            SDL_RenderSetClipRect(renderer, &currentClippingRectangle);
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
            SDL_RenderSetClipRect(renderer, NULL);
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_NONE:
            fprintf(stderr, "[plugin::yanui::backend] Warning: Skipping render command: none\n");
            break;

        case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
            Clay_ImageElementConfig *config = renderCommand->config.imageElementConfig;
            SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom(config->imageData, config->sourceDimensions.width, config->sourceDimensions.height, 32, 4 * config->sourceDimensions.width, SDL_PIXELFORMAT_RGBA32);
            if (!surf) {
                fprintf(stderr, "[plugin::yanui::backend] Error: could not create surface from image data: %s\n", SDL_GetError());
                continue;
                // exit(EXIT_FAILURE);
            }
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect rect = (SDL_Rect) {
                .x = (int) boundingBox.x,
                .y = (int) boundingBox.y,
                .w = (int) boundingBox.width,
                .h = (int) boundingBox.height,
            };
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surf);
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            Clay_BorderElementConfig *config = renderCommand->config.borderElementConfig;
            if (config->left.width > 0) {
                SDL_FRect rl = (SDL_FRect) {
                    .x = boundingBox.x,
                    .y = boundingBox.y + config->cornerRadius.topLeft,
                    .w = config->left.width,
                    .h = boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->left.color));
                SDL_RenderDrawRectF(renderer, &rl);
            }
            if (config->right.width > 0) {
                SDL_FRect rr = (SDL_FRect) {
                    .x = boundingBox.x + boundingBox.width - config->right.width,
                    .y = boundingBox.y + config->cornerRadius.topRight,
                    .w = config->right.width,
                    .h = boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->right.color));
                SDL_RenderDrawRectF(renderer, &rr);
            }
            if (config->bottom.width > 0) {
                SDL_FRect rb = (SDL_FRect) {
                    .x = boundingBox.x + config->cornerRadius.bottomLeft,
                    .y = boundingBox.y + boundingBox.height - config->bottom.width,
                    .w = boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight,
                    .h = config->bottom.width
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->bottom.color));
                SDL_RenderDrawRectF(renderer, &rb);
            }
            if (config->top.width > 0) {
                SDL_FRect rt = (SDL_FRect) {
                    .x = boundingBox.x + config->cornerRadius.topLeft,
                    .y = boundingBox.y,
                    .w = boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight,
                    .h = config->top.width
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->top.color));
                SDL_RenderDrawRectF(renderer, &rt);
            }

            if (config->cornerRadius.topLeft > 0) {
                SDL_FPoint center = (SDL_FPoint) {
                    .x = boundingBox.x + config->cornerRadius.topLeft,
                    .y = boundingBox.y + config->cornerRadius.topLeft
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->top.color));
                SDL_DrawCircle(renderer, center, config->top.width, 180, 270); 
            }
            if (config->cornerRadius.topRight > 0) {
                SDL_FPoint center = (SDL_FPoint) {
                    .x = boundingBox.x + boundingBox.width - config->cornerRadius.topRight,
                    .y = boundingBox.y + config->cornerRadius.topRight
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->top.color));
                SDL_DrawCircle(renderer, center, config->top.width, 270, 360);
            }
            if (config->cornerRadius.bottomLeft > 0) {
                SDL_FPoint center = (SDL_FPoint) {
                    .x = boundingBox.x + config->cornerRadius.bottomLeft,
                    .y = boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->bottom.color));
                SDL_DrawCircle(renderer, center, config->bottom.width, 90, 180);
            }
            if (config->cornerRadius.bottomRight > 0) {
                SDL_FPoint center = (SDL_FPoint) {
                    .x = boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight,
                    .y = boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight
                };
                SDL_SetRenderDrawColor(renderer, EXPAND_COLOR(config->bottom.color));
                SDL_DrawCircle(renderer, center, config->bottom.width, 0, 90);
            }
            break;
        }

        case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
            fprintf(stderr, "[plugin::yanui::backend] Warning: Custom render command doesn't support yet\n");
            break;
        
        default:
            fprintf(stderr, "[plugin::yanui::backend] Error: unhandled render command: %d\n", renderCommand->commandType);
            break;
        }
    }
}
#endif
