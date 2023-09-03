#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/shapes.h"
#include "../include/perspective.h"
#define BIG_NUM 500

int main(int argc, char* argv[]) {
    if (IMG_Init(IMG_INIT_JPG) != IMG_INIT_JPG) {
        printf("%s", SDL_GetError());
        return 1;
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("%s", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Rasterzier", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    Uint32 renderflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, renderflags);
    SDL_Surface* surface = IMG_Load("assets/crate.jpg");

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);

    Plane planes[] = {
        (Plane) {(Vector) {0, 0, 1}, -1},
        (Plane) {(Vector) {1/(sqrt(2)), 0, 1/sqrt(2)}, 0},
        (Plane) {(Vector) {-1/sqrt(2), 0, 1/sqrt(2)}, 0},
        (Plane) {(Vector) {0, 1/sqrt(2), 1/sqrt(2)}, 0},
        (Plane) {(Vector) {0, -1/sqrt(2), 1/sqrt(2)}, 0},
    };


    float buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            buffer[i][j] = 0;
        }
    }

    Light lights[] = {
        (Light) {0.2, 0, 0},
        (Light) {0.9, 1, (Vector) {-0.1, 0, 1.5}},
        (Light) {0.8, 1, (Vector) {0, 0, 1.5}}
    };

    Camera camera = {
        (Vector) {0, 0, 0},
        planes,
        2, 2, 1,
        lights, 3
    };
    int RED[] = {255, 0, 0};
    int GREEN[] = {0, 255, 0};
    int BLUE[] = {0, 0, 255};

    int CYAN[] = {255, 255, 0};
    int YELLOW[] = {255, 0, 255};
    int PURPLE[] = {0, 255, 255};

    Vector points[8] = {
        (Vector){ 0.5, 0.5, 0.5}, // 0
        (Vector){-0.5, 0.5, 0.5}, // 1
        (Vector){-0.5, -0.5, 0.5}, // 2
        (Vector){ 0.5, -0.5, 0.5}, // 3
        (Vector){ 0.5, 0.5, -0.5}, // 4
        (Vector){-0.5, 0.5, -0.5}, // 5
        (Vector){-0.5, -0.5, -0.5}, // 6
        (Vector){ 0.5, -0.5, -0.5} // 7
    };

    int ul[] = {0, 0};
    int ur[] = {1, 0};
    int dl[] = {0, 1};
    int dr[] = {1, 1};

    Triangle triangles[] = {
        (Triangle){
            0, 1, 2, 
            ur,
            dl,
            ul,
            RED
            },
        (Triangle){
            0, 2, 3, 
            ur,
            ul,
            dr,
            RED},
        (Triangle){4, 0, 3, 
            ul,
            ur,
            dr,
        GREEN},
        (Triangle){4, 3, 7, 
            ul,
            dr,
            dl,
        GREEN},
        (Triangle){5, 4, 7,
            ul,
            ur,
            dr,
        BLUE},
        (Triangle){5, 7, 6, 
            ul,
            dr,
            dl,
        BLUE},
        (Triangle){1, 5, 6, 
            ul,
            ur,
            dl,
        CYAN},
        (Triangle){1, 6, 2, 
            ul,
            dl,
            dr,
        CYAN},
        (Triangle){4, 5, 1, 
            dr,
            dl,
            ur,
        PURPLE},
        (Triangle){4, 1, 0, 
            dr,
            ur,
            ul,
        PURPLE},
        (Triangle){2, 6, 7, 
            ul,
            dl,
            dr,
        YELLOW},
        (Triangle){2, 7, 3, 
            ul,
            dr,
            ur,
        YELLOW}
    };

    Object cube = {points, triangles, 8, 12, get_longest(points, 8)};
    Instance c1 = {&cube, (Vector) {-1, 0, 2}, 1, 3.14/6, 0, 0};
    Instance c2 = {&cube, (Vector) {1, 0, 2}, 1, 3.14/3, 0, 0};
    c1.tex = surface;
    c2.tex = surface;

    SDL_Event event;
    while (1 > 0) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderInstance(renderer, &camera, &c1, buffer);
        renderInstance(renderer, &camera, &c2, buffer);
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(tex);
    IMG_Quit();
    SDL_Quit();
    return 0;
}