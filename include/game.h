#ifndef GAME_H

#define GAME_H
#include "all_headers.h"

typedef struct {
    SDL_Rect rect;
    bool active;
} Enemy;

void resetEnemy(Enemy* enemy);
int loadWords(const char *filename, char wordList[][50], int maxWords);
int play_game();
#endif