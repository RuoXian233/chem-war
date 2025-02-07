#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX_MATRIX_SIZE = 32;
#define N MAX_MATRIX_SIZE


typedef struct {
    float x;
    float y;
} Yanui_Vec2;

typedef struct {
    Yanui_Vec2 pos;
    Yanui_Vec2 centerPos;
    Yanui_Vec2 anchor;
} Yanui_Position;


typedef struct {
    Yanui_Position position;
    Yanui_Vec2 size;
} Yanui_Element;

typedef struct {
    int w, h;
    float **data;
} Matrix;


Matrix Matrix_Alloc(int w, int h, float val) {
    Matrix m = { .w = w, .h = h };
    m.data = (float **) malloc(sizeof(float *) * h);
    for (int i = 0; i < h; i++) {
        m.data[i] = (float *) malloc(sizeof(float) * w);
    }

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            m.data[i][j] = val;
        }
    }
}

void Matrix_Free(Matrix *m) {
    if (m->data) {
        for (int i = 0; i < m->h; i++) {
            free(m->data[i]);
        }
    }
    free(m->data);
    m->data = nullptr;
    m->h = 0;
    m->w = 0;
}


// typedef float Matrix[N][N];
