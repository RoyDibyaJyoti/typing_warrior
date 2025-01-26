#include "game.h"
#include "menu.h"
#include "button.h"

void resetEnemy(Enemy *enemy)
{
    enemy->rect.x = rand() % 1100;
    enemy->rect.y = -50;
    enemy->rect.w = 50;
    enemy->rect.h = 50;
    enemy->active = true;
}

// Load words from file into an array
int loadWords(const char *filename, char wordList[][50], int maxWords)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Failed to open file: %s\n", filename);
        return 0;
    }

    int count = 0;
    while (count < maxWords && fscanf(file, "%49s", wordList[count]) == 1)
    {
        count++;
    }

    fclose(file);
    return count;
}

int play_game()
{
    // Initialization
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow(
        "Typing Warrior",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 800,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 34, 97, 199, 255);
    Button end = create_button(renderer, "End Game", "img/btn.png", "img/btn_hover.png", 950, 700);

    TTF_Font *font = TTF_OpenFont("others/my_font.otf", 50);
    SDL_Color text_colour = {230, 209, 165, 255};

    SDL_Texture *background = IMG_LoadTexture(renderer, "img/spexpb.bmp");
    SDL_Texture *enemyTexture = IMG_LoadTexture(renderer, "img/asteroid.png");
    SDL_Texture *shooterTexture = IMG_LoadTexture(renderer, "img/_ship.png");

    // Shooter setup
    SDL_Rect shooter = {550, 720, 100, 50};
    SDL_Rect bullets[1000];
    int bulletCount = 0;

    // Enemy setup
    Enemy enemy;
    resetEnemy(&enemy);

    // Load words from file
    char wordList[100][50];
    int wordCount = loadWords("others/words.txt", wordList, 100);

    // Game variables
    char currentWord[50] = "";
    char userInput[50] = "";
    char pointText[50] = "Points: 0";
    int points = 0, lives = 3, speed = 1;
    bool typingMode = false, running = true;int t =200;

    srand((unsigned int)time(NULL));

    SDL_Rect wordRect = {550, 800, 0, 0};

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (!typingMode)
                {
                    if (event.key.keysym.sym == SDLK_SPACE)
                    {
                        if (bulletCount < 1000)
                        {
                            bullets[bulletCount].x = shooter.x + shooter.w / 2 - 5;
                            bullets[bulletCount].y = shooter.y;
                            bullets[bulletCount].w = 10;
                            bullets[bulletCount].h = 20;
                            bulletCount++;
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_LEFT)
                    {
                        shooter.x -= 20;
                        if (shooter.x < 0)
                            shooter.x = 0;
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                        shooter.x += 20;
                        if (shooter.x > 1100)
                            shooter.x = 1100;
                    }
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(userInput) > 0)
                {
                    userInput[strlen(userInput) - 1] = '\0';
                }
            }
            else if (event.type == SDL_TEXTINPUT && typingMode)
            {
                strcat(userInput, event.text.text);
                    printf("Current Word: %s, User Input: %s\n", currentWord, userInput);
                if (strcmp(userInput, currentWord) == 0)
                {
                    points += (wordRect.y)/25;
                    snprintf(pointText, sizeof(pointText), "Points: %d", points);
                    typingMode = false;
                    currentWord[0] = '\0';
                    userInput[0] = '\0'; 
                    if(points>700)speed++;
                    else if(points>650 && points<660)speed++;
                    else if(points>600 && points<620)speed++;
                    else if(points>550 && points<560)speed++;
                    else if(points>500 && points<520)speed++;
                    else if(points>450 && points<460)speed++;
                    else if(points>400 && points<420)speed++;
                    else if(points>350 && points<360)speed++;
                    else if(points>300 && points<320)speed++;
                    else if(points>250 && points<260)speed++;
                    else if(points>200 && points<220)speed++;
                    else if(points>150 && points<160)speed++;
                    else if(points>100 && points<120)speed++;
                    else if(points>50 && points<60)speed++;

                    resetEnemy(&enemy); // Reset the enemy after typing
                }
            }
            if(handle_button_event(&end, event)) return points;
        }
        // Game logic
        for (int i = 0; i < bulletCount; i++)
        {
            bullets[i].y -= 10;
        }

        int newBulletCount = 0;
        for (int i = 0; i < bulletCount; i++)
        {
            if (bullets[i].y > 0)
            {
                bullets[newBulletCount++] = bullets[i];
            }
        }
        bulletCount = newBulletCount;
        if (enemy.active)
        {
            enemy.rect.y += speed;
            for (int i = 0; i < bulletCount; i++)
            {
                if (SDL_HasIntersection(&enemy.rect, &bullets[i]))
                {
                    enemy.active = false;
                    typingMode = true;
                    strcpy(currentWord, wordList[rand() % wordCount]);
                    bulletCount--; // Remove the bullet
                    for (int j = i; j < bulletCount; j++)
                    {
                        bullets[j] = bullets[j + 1];
                    }
                    break;
                }
            }

            if (enemy.rect.y > 800)
            {
                enemy.active = false;
                lives--;
                if (lives <= 0)
                {
                    running = false;
                }
                else
                {
                    resetEnemy(&enemy);
                }
            }
        }
        else if (!typingMode)
        {
            resetEnemy(&enemy);
            enemy.active = true;
        }
        // Rendering
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, background, NULL, NULL);

        if (enemy.active)
        {
            SDL_RenderCopy(renderer, enemyTexture, NULL, &enemy.rect);
        }

        for (int i = 0; i < bulletCount; i++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &bullets[i]);
        }

        SDL_RenderCopy(renderer, shooterTexture, NULL, &shooter);

        if (typingMode)
        {
            if (wordRect.y == 800)
            {
                SDL_Surface *tempSurface = TTF_RenderText_Blended(font, currentWord, text_colour);
                wordRect.w = tempSurface->w;
                wordRect.h = tempSurface->h;
                SDL_FreeSurface(tempSurface);
            }

            SDL_Surface *surface = TTF_RenderText_Blended(font, currentWord, text_colour);
            SDL_Texture *wordTexture = SDL_CreateTextureFromSurface(renderer, surface);

            wordRect.y -= speed; // Move the word upward
            SDL_RenderCopy(renderer, wordTexture, NULL, &wordRect);

            SDL_DestroyTexture(wordTexture);
            SDL_FreeSurface(surface);

            if (wordRect.y + wordRect.h < 0)
            {
                typingMode = false;
                currentWord[0] = '\0';
                userInput[0] = '\0'; 
                wordRect.y = 800;
            }
        }

        SDL_Surface *pointSurface = TTF_RenderText_Blended(font, pointText, text_colour);
        SDL_Texture *pointTexture = SDL_CreateTextureFromSurface(renderer, pointSurface);
        SDL_Rect pointRect = {10, 10, pointSurface->w, pointSurface->h};
        SDL_RenderCopy(renderer, pointTexture, NULL, &pointRect);
        SDL_DestroyTexture(pointTexture);
        SDL_FreeSurface(pointSurface);
        render_button(renderer, &end);

        SDL_RenderPresent(renderer);

        SDL_Delay(25);
    }

    printf("Final Points: %d\n", points);
    // set_score(points);

    // Cleanup
    destroy_button(&end);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(enemyTexture);
    SDL_DestroyTexture(shooterTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    // show_menu();

    return points;
}
