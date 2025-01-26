#include "button.h"

Button create_button(SDL_Renderer* renderer, const char* text, const char* imagePath, const char* hoverImagePath, int x, int y) {
    Button button;
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return button; 
    }

    button.normalTexture = IMG_LoadTexture(renderer, imagePath);
    if (!button.normalTexture) {
        printf("Failed to load normal texture: %s\n", IMG_GetError());
    }
    
    button.hoverTexture = IMG_LoadTexture(renderer, hoverImagePath);
    if (!button.hoverTexture) {
        printf("Failed to load hover texture: %s\n", IMG_GetError());
    }

    button.rect = (SDL_Rect){x, y, 200, 50};
    TTF_Init();


    TTF_Font* font = TTF_OpenFont("others/my_font.otf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
    }

    SDL_Color textColor = {255, 255, 255, 255};  // White text color
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, textColor);
    button.textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    button.textRect = (SDL_Rect){x + 40, y + 10, textSurface->w, textSurface->h};

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);

    return button;
}

void render_button(SDL_Renderer* renderer, Button* button) {
    SDL_Texture* texture = button->normalTexture;
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    if (SDL_PointInRect(&(SDL_Point){x, y}, &button->rect)) {
        texture = button->hoverTexture;
    }
    
    SDL_RenderCopy(renderer, texture, NULL, &button->rect); 
    SDL_RenderCopy(renderer, button->textTexture, NULL, &button->textRect); 
}

int handle_button_event(Button* button, SDL_Event e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && SDL_PointInRect(&(SDL_Point){e.button.x, e.button.y}, &button->rect)) {
        return 1;
    }
    return 0;
}

void destroy_button(Button* button) {
    SDL_DestroyTexture(button->normalTexture);
    SDL_DestroyTexture(button->hoverTexture);
    SDL_DestroyTexture(button->textTexture);
}
