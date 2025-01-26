#ifndef MENU_H
#define MENU_H

#include "all_headers.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    SDL_Texture *title;
    TTF_Font *font;
} Menu;

void show_menu();
bool init_menu(Menu* menu);
void destroy_menu(Menu* menu);
bool load_media(Menu* menu);
#endif