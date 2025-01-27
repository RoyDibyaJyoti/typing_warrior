#include "game.h"
#include "menu.h"
#include "button.h"
#include "score.h"

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

int readHighestScore(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        return 0;
    }

    int highestScore = 0;
    if (fscanf(file, "%d", &highestScore) != 1)
    {
        highestScore = 0;
    }
    fclose(file);

    return highestScore;
}

void writeHighestScore(const char *filename, int score){
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

Score play_game(){
    // Initialization
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Typing Warrior", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 34, 97, 199, 255);

    TTF_Font *font = TTF_OpenFont("others/my_font.otf", 50);
    SDL_Color text_colour = {230, 209, 165, 255};

    SDL_Texture *background = IMG_LoadTexture(renderer, "img/spexpb.bmp");
    SDL_Texture *enemyTexture = IMG_LoadTexture(renderer, "img/asteroid.png");
    SDL_Texture *shooterTexture = IMG_LoadTexture(renderer, "img/_ship.png");

    Button end = create_button(renderer, "End Game", "img/btn.png", "img/btn_hover.png", 950, 700);

    // Shooter setup
    SDL_Rect shooter = {550, 720, 100, 50};
    SDL_Rect bullets[1000];
    int bulletCount = 0;
    const int SHOOTER_SPEED = 5;

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
    int lives = 3, speed = 1;
    bool typingMode = false, running = true;
    Uint32 lastShot = 0;
    const Uint32 SHOT_DELAY = 250; // Minimum delay between shots in milliseconds

    SDL_Rect wordRect = {550, 800, 0, 0};

    // Add user input display position
    SDL_Rect inputDisplayRect = {1100, 50, 0, 0};

    const char *scoreFile = "others/highestScore.txt";
    Score score = {
        .correct_words = 0,
        .high_score = readHighestScore(scoreFile),
        .wrong_words = 0,
        .score = 0
    };

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN && !typingMode)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - lastShot >= SHOT_DELAY && bulletCount < 1000)
                    {
                        bullets[bulletCount].x = shooter.x + shooter.w / 2 - 5;
                        bullets[bulletCount].y = shooter.y;
                        bullets[bulletCount].w = 10;
                        bullets[bulletCount].h = 20;
                        bulletCount++;
                        lastShot = currentTime;
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN && typingMode)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(userInput) > 0)
                {
                    userInput[strlen(userInput) - 1] = '\0';
                }
            }
            else if (event.type == SDL_TEXTINPUT && typingMode)
            {
                strcat(userInput, event.text.text);
                if (strcmp(userInput, currentWord) == 0)
                {
                    score.score += (wordRect.y) / 25;
                    score.correct_words++;
                    snprintf(pointText, sizeof(pointText), "Points: %d", score.score);
                    typingMode = false;
                    currentWord[0] = '\0';
                    userInput[0] = '\0';
                    if(score.score > 50) speed = 1 + score.score / 50; // Smoother speed progression
                    resetEnemy(&enemy);
                }
                else if (strlen(userInput) >= strlen(currentWord))
                {
                    score.score -= 10;
                    score.wrong_words++;
                    snprintf(pointText, sizeof(pointText), "Points: %d", score.score);
                    typingMode = false;
                    currentWord[0] = '\0';
                    userInput[0] = '\0';
                    resetEnemy(&enemy);
                }
            }
            if(handle_button_event(&end, event)) return score;
        }

        // Handle keyboard state for continuous movement
        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        if (!typingMode) {
            if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
                shooter.x -= SHOOTER_SPEED;
                if (shooter.x < 0) shooter.x = 0;
            }
            if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
                shooter.x += SHOOTER_SPEED;
                if (shooter.x > 1100) shooter.x = 1100;
            }
        }

        // Update bullets
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

        // Enemy logic
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
                    bulletCount--;
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

            wordRect.y -= speed;
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

        // Render user input text at top-right
        SDL_Surface *inputSurface = TTF_RenderText_Blended(font, userInput, text_colour);
        if (inputSurface) {
            SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
            inputDisplayRect.w = inputSurface->w;
            inputDisplayRect.h = inputSurface->h;
            inputDisplayRect.x = 1150 - inputSurface->w; // Right align
            
            SDL_RenderCopy(renderer, inputTexture, NULL, &inputDisplayRect);
            SDL_FreeSurface(inputSurface);
            SDL_DestroyTexture(inputTexture);
        }

        // Render score
        SDL_Surface *pointsurface = TTF_RenderText_Blended(font, pointText, text_colour);
        SDL_Texture *pointTexture = SDL_CreateTextureFromSurface(renderer, pointsurface);
        SDL_Rect pointRect = {10, 50, pointsurface->w, pointsurface->h};
        SDL_RenderCopy(renderer, pointTexture, NULL, &pointRect);
        SDL_DestroyTexture(pointTexture);
        SDL_FreeSurface(pointsurface);
        
        render_button(renderer, &end);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); //  60 FPS
    }

    if (score.score > score.high_score)
    {
        writeHighestScore(scoreFile, score.score);
    }

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

    return score;
}
