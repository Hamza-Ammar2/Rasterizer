#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "structs.h"


Point Vec2Point(Camera* camera, Vector* point);
Vector add(Vector v1, Vector v2);
Vector sub(Vector v1, Vector v2);
float getLength(Vector v);
float dot(Vector v1, Vector v2);
Vector cross(Vector v1, Vector v2);
Vector scale(Vector v, float s);
float get_longest(Vector* points, int len);
Vector rotateXZ(Vector v, double phi);
Vector rotateYZ(Vector v, double theta);
Vector rotateXY(Vector v, double a);
void renderObj(SDL_Renderer* renderer, Camera* camera, Object obj);
float calcIntensity(Vector normal, Vector point, Light* lights, int len_l);
void renderInstance(SDL_Renderer* renderer, Camera* camera, Instance* instance, float buffer[500][500]);