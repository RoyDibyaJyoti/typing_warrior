#include "menu.h"
#include "button.h"
#include "game.h"
#include "score.h"

void show_menu(){
    Menu menu;

    if(!init_menu(&menu)){
        printf("ERROR initializing menu...\n");
        return;
    }

    Button play = create_button(menu.renderer, "  Play", "img/btn.png", "img/btn_hover.png", 500, 300);
    Button exit = create_button(menu.renderer, "  Exit", "img/btn.png", "img/btn_hover.png", 500, 400);

    if(!load_media(&menu)){
        printf("ERROR loading media...\n");
        return;
    }

    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
                break;
            }
            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
            if(handle_button_event(&play, event)){
                printf("Play Button Clicked\n");
                destroy_button(&play);
                destroy_button(&exit);
                destroy_menu(&menu);
                show_score(play_game());
                return;
            }
            if(handle_button_event(&exit, event)){
                printf("Exit Button Clicked\n");
                running = false;
            }
        }
        SDL_RenderClear(menu.renderer);
        SDL_RenderCopy(menu.renderer, menu.background, NULL, NULL);
        
        render_button(menu.renderer, &play);
        render_button(menu.renderer, &exit);
        
        SDL_RenderPresent(menu.renderer);
        SDL_Delay(1000 / 60);
    }
    destroy_button(&play);
    destroy_button(&exit);
    destroy_menu(&menu);
    return;
}

bool init_menu(Menu* menu){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf("Something went wrong with SDL_Init: %s\n", SDL_GetError());
        return false;
    }
    menu->window = SDL_CreateWindow(
        "Typing Warrior - Menu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 800,
        SDL_WINDOW_SHOWN
    );
    if(menu->window == NULL){
        printf("Something went wrong with SDL_CreateWindow: %s\n", SDL_GetError());
        return false;
    }
    menu->renderer = SDL_CreateRenderer(menu->window, -1, SDL_RENDERER_ACCELERATED);
    if(menu->renderer == NULL){
        printf("Something went wrong with SDL_CreateRenderer: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool load_media(Menu* menu){
    if(TTF_Init() < 0){
        printf("Something went wrong with TTF_Init: %s\n", TTF_GetError());
        return false;
    }
    menu->font = TTF_OpenFont("others/my_font.otf", 24);
    if(menu->font == NULL){
        printf("Something went wrong with TTF_OpenFont: %s\n", TTF_GetError());
        return false;
    }

    if(IMG_Init(IMG_INIT_PNG) == 0){
        printf("Something went wrong with IMG_Init: %s\n", IMG_GetError());
        return false;
    }
    menu->background = IMG_LoadTexture(menu->renderer, "img/spexpb.bmp");
    if(menu->background == NULL){
        printf("Something went wrong with IMG_LoadTexture: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

void destroy_menu(Menu* menu){
    SDL_DestroyRenderer(menu->renderer);
    SDL_DestroyWindow(menu->window);
    TTF_CloseFont(menu->font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
