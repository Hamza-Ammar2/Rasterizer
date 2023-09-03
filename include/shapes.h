#ifndef SHAPES_H
#define SHAPES_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "structs.h"

#define SCREEN_WIDTH  500
#define SCREEN_HEIGHT 500
#define VW 2
#define VH 2
#define VD 1


float* Interpolate(float i0, float d0, float i1, float d1);

void DrawLine(SDL_Renderer* renderer, Point P0, Point P1, int* color);
void DrawTriangle(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color);
void FillTriangle(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color);
void FillTriangleSmart(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color, float buffer[500][500]);
void ShadeTriangleSmart(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color, Vector normal, Light* lights, int len_l, float buffer[500][500]);
void ShadeTex(SDL_Renderer* renderer, SDL_Surface* tex, Point* P0, Point* P1, Point* P2, int* color, Vector normal, Light* lights, int len_l,  float buffer[500][500]);
#endif