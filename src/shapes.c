#include "../include/shapes.h"
#include "../include/perspective.h"

SDL_Color GetPixelColor(const SDL_Surface* pSurface, const int X, const int Y)
{
	// Bytes per pixel
	const Uint8 Bpp = pSurface->format->BytesPerPixel;

	/*
	Retrieve the address to a specific pixel
	pSurface->pixels	= an array containing the SDL_Surface' pixels
	pSurface->pitch		= the length of a row of pixels (in bytes)
	X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
	*/
	Uint8* pPixel = (Uint8*)pSurface->pixels + Y * pSurface->pitch + X * Bpp;

	Uint32 PixelData = *(Uint32*)pPixel;

	SDL_Color Color = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

	// Retrieve the RGB values of the specific pixel
	SDL_GetRGB(PixelData, pSurface->format, &Color.r, &Color.g, &Color.b);

	return Color;
}




float getCos(Vector v1, Vector v2) {
    return dot(v1, v2)/sqrt(dot(v1, v1)*dot(v2, v2));
}


Vector getVec(float x, float y, float _z) {
    float z = 1/_z;
    x -= SCREEN_WIDTH/2;
    y = -y + SCREEN_HEIGHT/2;

    return (Vector) {
        VW*z*x/(SCREEN_WIDTH*VD),
        VH*z*y/(SCREEN_HEIGHT*VD),
        z
    };
}


float* Interpolate(float i0, float d0, float i1, float d1) {
    int len = round(i1 - i0);
    float* arr = malloc(sizeof(float)*len);
    float d = d0;

    float a = (d1 - d0) / (i1 - i0);
    for (int i = 0; i < len; i++){
        arr[i] = d;
        d += a;
    } 

    return arr;
}

void DrawLine(SDL_Renderer* renderer, Point P0, Point P1, int* color) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);

    if (abs(P1.x - P0.x) > abs(P1.y - P0.y)) {
        if (P1.x < P0.x) {
            Point dum = P1;
            P1 = P0;
            P0 = dum;
        }

        float* ys = Interpolate(P0.x, P0.y, P1.x, P1.y);
        int len = round(P1.x - P0.x);
        for (int i = 0; i < len; i++) {
            SDL_RenderFillRect(renderer, &((SDL_Rect) {P0.x+i, ys[i], 1, 1}));
        }

        free(ys);
    } else {
        if (P1.y < P0.y) {
            Point dum = P1;
            P1 = P0;
            P0 = dum;
        }

        float* xs = Interpolate(P0.y, P0.x, P1.y, P1.x);
        int len = round(P1.y - P0.y);
        for (int i = 0; i < len; i++) {
            SDL_RenderFillRect(renderer, &((SDL_Rect) {xs[i], P0.y+i, 1, 1}));
        }

        free(xs);
    }
}


void DrawTriangle(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color) {
    DrawLine(renderer, *P0, *P1, color);
    DrawLine(renderer, *P1, *P2, color);
    DrawLine(renderer, *P2, *P0, color);
}

void FillTriangle(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255); 

    if (P1->y < P0->y) {
        Point* dum = P1;
        P1 = P0;
        P0 = dum;
    }
    if (P2->y < P0->y) {
        Point* dum = P2;
        P2 = P0;
        P0 = dum;
    }
    if (P2->y < P1->y) {
        Point* dum = P2;
        P2 = P1;
        P1 = dum;
    }

    float* x01 = Interpolate(P0->y, P0->x, P1->y, P1->x);
    float* x12 = Interpolate(P1->y, P1->x, P2->y, P2->x);
    float* x02 = Interpolate(P0->y, P0->x, P2->y, P2->x);

    int len1 = (int) (abs(P0->y - P1->y));
    int len2 = (int) (abs(P1->y - P2->y));
    int len02 = (int) (abs(P0->y - P2->y));

    float* x012 = malloc(sizeof(float)*(len1 + len2));
    for (int i = 0; i < len1; i++){
        x012[i] = x01[i];
    }
    for (int i = 0; i < len2; i++){
        x012[len1 + i] = x12[i];
    }

    float* x_left;
    float* x_right;
    int m = floor(len02/2);

    if (x02[m] < x012[m]) {
        x_left = x02;
        x_right = x012;
    } else {
        x_left = x012;
        x_right = x02;
    }

    for (int i = 0; i < len02; i++) {
        int l = (int) (x_right[i] - x_left[i]);
        int st = (int) x_left[i];
        for (int j = 0; j < l; j++) {
            SDL_RenderFillRect(renderer, &((SDL_Rect) {
                st + j,
                P0->y + i,
                1, 1
            }));
        }
    }

    free(x01);
    free(x012);
    free(x02);
    free(x12);
}


void FillTriangleSmart(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color, float buffer[500][500]) {
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255); 

    if (P1->y < P0->y) {
        Point* dum = P1;
        P1 = P0;
        P0 = dum;
    }
    if (P2->y < P0->y) {
        Point* dum = P2;
        P2 = P0;
        P0 = dum;
    }
    if (P2->y < P1->y) {
        Point* dum = P2;
        P2 = P1;
        P1 = dum;
    }

    float* x01 = Interpolate(P0->y, P0->x, P1->y, P1->x);
    float* x12 = Interpolate(P1->y, P1->x, P2->y, P2->x);
    float* x02 = Interpolate(P0->y, P0->x, P2->y, P2->x);

    float* z01 = Interpolate(P0->y, P0->z, P1->y, P1->z);
    float* z12 = Interpolate(P1->y, P1->z, P2->y, P2->z);
    float* z02 = Interpolate(P0->y, P0->z, P2->y, P2->z);

    int len1 = (int) (abs(P0->y - P1->y));
    int len2 = (int) (abs(P1->y - P2->y));
    int len02 = (int) (abs(P0->y - P2->y));

    float* x012 = malloc(sizeof(float)*(len1 + len2));
    float* z012 = malloc(sizeof(float)*(len1 + len2));

    for (int i = 0; i < len1; i++){
        x012[i] = x01[i];
        z012[i] = z01[i];
    }
    for (int i = 0; i < len2; i++){
        x012[len1 + i] = x12[i];
        z012[len1 + i] = z12[i];
    }

    float* x_left;
    float* x_right;
    float* z_left;
    float* z_right;

    int m = floor(len02/2);

    if (x02[m] < x012[m]) {
        x_left = x02;
        x_right = x012;

        z_left = z02;
        z_right = z012;
    } else {
        x_left = x012;
        x_right = x02;

        z_left = z012;
        z_right = z02;
    }

    for (int i = 0; i < len02; i++) {
        int l = (int) (x_right[i] - x_left[i]);
        int st = (int) x_left[i];

        int y = ((int) P0->y) + i;
        float* z_segment = Interpolate(x_left[i], z_left[i], x_right[i], z_right[i]);
        if (l > abs(P0->x - P1->x) && l > abs(P1->x - P2->x)) continue;

        for (int j = 0; j < l; j++) {
            float z = z_segment[j];
            if (z >= buffer[y][st + j]) {
                buffer[y][st+j] = z;

                SDL_RenderFillRect(renderer, &((SDL_Rect) {
                    st + j,
                    P0->y + i,
                    2, 2
                }));
            }
        }
        free(z_segment);
    }

    

    free(x01);
    free(x012);
    free(x02);
    free(x12);

    free(z01);
    free(z012);
    free(z02);
    free(z12);
}



void ShadeTriangleSmart(SDL_Renderer* renderer, Point* P0, Point* P1, Point* P2, int* color, Vector normal, Light* lights, int len_l, float buffer[500][500]) {
    if (P1->y < P0->y) {
        Point* dum = P1;
        P1 = P0;
        P0 = dum;
    }
    if (P2->y < P0->y) {
        Point* dum = P2;
        P2 = P0;
        P0 = dum;
    }
    if (P2->y < P1->y) {
        Point* dum = P2;
        P2 = P1;
        P1 = dum;
    }

    float* x01 = Interpolate(P0->y, P0->x, P1->y, P1->x);
    float* x12 = Interpolate(P1->y, P1->x, P2->y, P2->x);
    float* x02 = Interpolate(P0->y, P0->x, P2->y, P2->x);

    float* z01 = Interpolate(P0->y, P0->z, P1->y, P1->z);
    float* z12 = Interpolate(P1->y, P1->z, P2->y, P2->z);
    float* z02 = Interpolate(P0->y, P0->z, P2->y, P2->z);

    int len1 = (int) (abs(P0->y - P1->y));
    int len2 = (int) (abs(P1->y - P2->y));
    int len02 = (int) (abs(P0->y - P2->y));

    float* x012 = malloc(sizeof(float)*(len1 + len2));
    float* z012 = malloc(sizeof(float)*(len1 + len2));

    for (int i = 0; i < len1; i++){
        x012[i] = x01[i];
        z012[i] = z01[i];
    }
    for (int i = 0; i < len2; i++){
        x012[len1 + i] = x12[i];
        z012[len1 + i] = z12[i];
    }

    float* x_left;
    float* x_right;
    float* z_left;
    float* z_right;

    int m = floor(len02/2);

    if (x02[m] < x012[m]) {
        x_left = x02;
        x_right = x012;

        z_left = z02;
        z_right = z012;
    } else {
        x_left = x012;
        x_right = x02;

        z_left = z012;
        z_right = z02;
    }

    for (int i = 0; i < len02; i++) {
        int l = (int) (x_right[i] - x_left[i]);
        if (l > abs(P0->x - P1->x) && l > abs(P1->x - P2->x)) continue;
        int st = (int) x_left[i];

        int y = ((int) P0->y) + i;
        float* z_segment = Interpolate(x_left[i], z_left[i], x_right[i], z_right[i]);
        int x;

        for (int j = 0; j < l; j++) {
            x = st + j;
            float z = z_segment[j];
            if (z < 0) z*= -1;
            if (z >= buffer[y][x]) {
                buffer[y][x] = z;
                float h = calcIntensity(normal, getVec(x, y, z), lights, len_l);
                if (h < 0) h *= -1;
                int r = color[0]*h; int g = color[1]*h; int b = color[2]*h;
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;

                SDL_SetRenderDrawColor(renderer, 
                    (Uint8) (r), 
                    (Uint8) (g), 
                    (Uint8) (b), 255); 

                SDL_RenderFillRect(renderer, &((SDL_Rect) {
                    x,
                    y,
                    2, 2
                }));
            }
        }
        free(z_segment);
    }


    free(x01);
    free(x012);
    free(x02);
    free(x12);

    free(z01);
    free(z012);
    free(z02);
    free(z12);
}


void ShadeTex(SDL_Renderer* renderer, SDL_Surface* tex, Point* P0, 
Point* P1, Point* P2, int* color, 
Vector normal,
Light* lights,
int len_l,
float buffer[500][500]) {
    if (P1->y < P0->y) {
        Point* dum = P1;
        P1 = P0;
        P0 = dum;
    }
    if (P2->y < P0->y) {
        Point* dum = P2;
        P2 = P0;
        P0 = dum;
    }
    if (P2->y < P1->y) {
        Point* dum = P2;
        P2 = P1;
        P1 = dum;
    }

    float* x01 = Interpolate(P0->y, P0->x, P1->y, P1->x);
    float* x12 = Interpolate(P1->y, P1->x, P2->y, P2->x);
    float* x02 = Interpolate(P0->y, P0->x, P2->y, P2->x);

    float* z01 = Interpolate(P0->y, P0->z, P1->y, P1->z);
    float* z12 = Interpolate(P1->y, P1->z, P2->y, P2->z);
    float* z02 = Interpolate(P0->y, P0->z, P2->y, P2->z);


    int len1 = (int) (abs(P0->y - P1->y));
    int len2 = (int) (abs(P1->y - P2->y));
    int len02 = (int) (abs(P0->y - P2->y));

    float* v01; float* v12; float* v02; float* u01; float* u12; float* u02;
    v01 = Interpolate(P0->y, P0->tx[0]*P0->z, P1->y, P1->tx[0]*P1->z);
    v12 = Interpolate(P1->y, P1->tx[0]*P1->z, P2->y, P2->tx[0]*P2->z);
    v02 = Interpolate(P0->y, P0->tx[0]*P0->z, P2->y, P2->tx[0]*P2->z);

    u01 = Interpolate(P0->y, P0->tx[1]*P0->z, P1->y, P1->tx[1]*P1->z);
    u12 = Interpolate(P1->y, P1->tx[1]*P1->z, P2->y, P2->tx[1]*P2->z);
    u02 = Interpolate(P0->y, P0->tx[1]*P0->z, P2->y, P2->tx[1]*P2->z);

    float* x012 = malloc(sizeof(float)*(len1 + len2));
    float* z012 = malloc(sizeof(float)*(len1 + len2));
    float* v012 = malloc(sizeof(float)*(len1 + len2));
    float* u012 = malloc(sizeof(float)*(len1 + len2));

    for (int i = 0; i < len1; i++){
        x012[i] = x01[i];
        z012[i] = z01[i];
        v012[i] = v01[i];
        u012[i] = u01[i];
    }
    for (int i = 0; i < len2; i++){
        x012[len1 + i] = x12[i];
        z012[len1 + i] = z12[i];
        v012[len1 + i] = v12[i];
        u012[len1 + i] = u12[i];
    }

    float* x_left;
    float* x_right;
    float* z_left;
    float* z_right;
    float* v_left;
    float* v_right;
    float* u_left;
    float* u_right;

    int m = floor(len02/2);

    if (x02[m] < x012[m]) {
        x_left = x02;
        x_right = x012;

        z_left = z02;
        z_right = z012;

        u_left = u02;
        u_right = u012;

        v_left = v02;
        v_right = v012;
    } else {
        x_left = x012;
        x_right = x02;

        z_left = z012;
        z_right = z02;

        u_left = u012;
        u_right = u02;

        v_left = v012;
        v_right = v02;
    }

    for (int i = 0; i < len02; i++) {
        int l = (int) (x_right[i] - x_left[i]);
        if (l > abs(P0->x - P1->x) && l > abs(P1->x - P2->x)) continue;
        int st = (int) x_left[i];

        int y = ((int) P0->y) + i;

        float* v_segment = Interpolate(x_left[i], v_left[i], x_right[i], v_right[i]);
        float* u_segment = Interpolate(x_left[i], u_left[i], x_right[i], u_right[i]);
        float* z_segment = Interpolate(x_left[i], z_left[i], x_right[i], z_right[i]);
        int x;

        for (int j = 0; j < l; j++) {
            x = st + j;
            float z = z_segment[j];
            if (z < 0) z *= -1;
            if (z >= buffer[y][x]) {
                buffer[y][x] = z;
                float h = calcIntensity(normal, getVec(x, y, z), lights, len_l);
                if (h < 0) h *= -1;
                
                float v = v_segment[j]/z;
                float u = u_segment[j]/z;
                if (u < 0) u *= -1;
                if (v < 0) v *= -1;
                
                SDL_Color color = GetPixelColor(tex, v*512, u*512);
                int r = color.r*h; int g = color.g*h; int b = color.b*h;
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;

                SDL_SetRenderDrawColor(renderer, 
                    (Uint8) (r), 
                    (Uint8) (g), 
                    (Uint8) (b), 255); 

                SDL_RenderFillRect(renderer, &((SDL_Rect) {
                    x,
                    y,
                    2, 2
                }));
            }
        }
        free(v_segment);
        free(u_segment);
        free(z_segment);
    }

    free(x01);
    free(x012);
    free(x02);
    free(x12);

    free(z01);
    free(z012);
    free(z02);
    free(z12);

    free(v01);
    free(v012);
    free(v02);
    free(v12);

    free(u01);
    free(u012);
    free(u02);
    free(u12);
}