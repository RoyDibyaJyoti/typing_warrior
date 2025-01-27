#ifndef SCORE_H
#define SCORE_H

#include "all_headers.h"

typedef struct {
    int score;
    int high_score;
    int correct_words;
    int wrong_words;
} Score;

void show_score(Score score);
void writeHighestScore(const char *filename, int score);
int readHighestScore(const char *filename);
#endif