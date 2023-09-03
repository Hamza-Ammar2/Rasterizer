#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

typedef struct Point {
    float x;
    float y;
    float z;
    int* tx;
} Point;

typedef struct Triangle {
    int v1;
    int v2;
    int v3;

    int* v1_tx;
    int* v2_tx;
    int* v3_tx;
    int* color;
} Triangle;

typedef struct Vector {
    float x;
    float y;
    float z;
} Vector;

typedef struct Plane {
    Vector normal;
    float d;
} Plane;

typedef struct Light {
    float intensity;
    int type;
    Vector direction;
} Light;

typedef struct Camera {
    Vector position;
    Plane* planes;
    int Vw;
    int Vh;
    int d;
    Light* lights;
    int len_l;
} Camera;

typedef struct Object {
    Vector* points;
    Triangle* triangles;
    int len_v;
    int len_tr;
    float r;
} Object;


typedef struct Instance {
    Object* model;
    Vector position;
    float scale;
    double phi;
    double theta;
    double a;
    SDL_Surface* tex;
} Instance;
#endif