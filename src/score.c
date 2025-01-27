#include "score.h"
#include "menu.h"


void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius) {
    SDL_Rect center = {
        rect->x + radius,
        rect->y + radius,
        rect->w - 2 * radius,
        rect->h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &center);

    SDL_Rect horizontal = {
        rect->x + radius,
        rect->y,
        rect->w - 2 * radius,
        radius
    };
    SDL_RenderFillRect(renderer, &horizontal);
    horizontal.y = rect->y + rect->h - radius;
    SDL_RenderFillRect(renderer, &horizontal);

    SDL_Rect vertical = {
        rect->x,
        rect->y + radius,
        radius,
        rect->h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &vertical);
    vertical.x = rect->x + rect->w - radius;
    SDL_RenderFillRect(renderer, &vertical);

    for (int i = 0; i < 4; i++) {
        int cx = (i % 2 == 0) ? rect->x + radius : rect->x + rect->w - radius;
        int cy = (i / 2 == 0) ? rect->y + radius : rect->y + rect->h - radius;

        for (int x = 0; x <= radius; x++) {
            for (int y = 0; y <= radius; y++) {
                if (x*x + y*y <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, 
                        (i % 2 == 0) ? cx - x : cx + x, 
                        (i / 2 == 0) ? cy - y : cy + y
                    );
                }
            }
        }
    }
}


void show_score(Score score) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL Initialization Error: %s\n", SDL_GetError());
        return;
    }

    if (TTF_Init() == -1) {
        printf("TTF Initialization Error: %s\n", TTF_GetError());
        SDL_Quit();
        return;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Typing Warrior - Score",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 800,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Window Creation Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer Creation Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    SDL_Surface* background_surface = SDL_LoadBMP("img/spexpb.bmp");
    if (!background_surface) {
        printf("Background Image Load Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);
    SDL_FreeSurface(background_surface);
    if (!background_texture) {
        printf("Background Texture Creation Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    TTF_Font* font = TTF_OpenFont("others/my_font.otf", 30);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        SDL_DestroyTexture(background_texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    char score_str[300];
    snprintf(score_str, sizeof(score_str), 
             "Game Over!\nFinal Points: %d\nCorrect Words: %d\nWrong Words: %d\nHighest Score: %d%s", 
             score.score, score.correct_words, score.wrong_words, score.high_score, 
             (score.score > score.high_score) ? "\nCongratulations! New High Score!" : "");

    SDL_Surface* surface_score = TTF_RenderText_Blended_Wrapped(font, score_str, (SDL_Color){0, 0, 0}, 1000);
    SDL_Surface* surface_prompt = TTF_RenderText_Blended_Wrapped(font, "\n\n\nPress any key if you agree\nShawon sir and Taj sir are SHERA!!!", (SDL_Color){0, 0, 0}, 1000);

    if (!surface_score || !surface_prompt) {
        printf("Text Rendering Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture_score = SDL_CreateTextureFromSurface(renderer, surface_score);
    SDL_Texture* texture_prompt = SDL_CreateTextureFromSurface(renderer, surface_prompt);

    if (!texture_score || !texture_prompt) {
        printf("Texture Creation Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Rect text_bg_rect = {
        250,
        200, 
        750, 
        500  
    };

    SDL_Rect rect_score = {
        text_bg_rect.x + 50, 
        text_bg_rect.y + 50, 
        surface_score->w, 
        surface_score->h
    };
    SDL_Rect rect_prompt = {
        text_bg_rect.x + 50, 
        text_bg_rect.y + 250, 
        surface_prompt->w, 
        surface_prompt->h
    };

    SDL_RenderCopy(renderer, background_texture, NULL, NULL);  
    

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    draw_rounded_rect(renderer, &text_bg_rect, 50); 

    SDL_RenderCopy(renderer, texture_score, NULL, &rect_score);
    SDL_RenderCopy(renderer, texture_prompt, NULL, &rect_prompt);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                running = 0;
            }
        }
    }

    SDL_FreeSurface(surface_score);
    SDL_FreeSurface(surface_prompt);
    SDL_DestroyTexture(texture_score);
    SDL_DestroyTexture(texture_prompt);
    SDL_DestroyTexture(background_texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    show_menu();
}