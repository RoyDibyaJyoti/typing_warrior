#ifndef BUTTON_H
#define BUTTON_H

#include "all_headers.h"

typedef struct Button {
    SDL_Texture* normalTexture;   
    SDL_Texture* hoverTexture;  
    SDL_Texture* textTexture;   
    SDL_Rect rect;  
    SDL_Rect textRect; 
} Button;


Button create_button(SDL_Renderer* renderer, const char* text, const char* imagePath, const char* hoverImagePath, int x, int y);


void render_button(SDL_Renderer* renderer, Button* button);


int handle_button_event(Button* button, SDL_Event e);


void destroy_button(Button* button);

#endif
