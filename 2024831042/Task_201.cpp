#include <SDL2/SDL.h>
#include <stdio.h>

// Screen and grid settings
#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 540
#define CELL_SIZE     20
#define GRID_COLS     SCREEN_WIDTH  / CELL_SIZE   // 54
#define GRID_ROWS     SCREEN_HEIGHT / CELL_SIZE   // 27

// Global SDL variables
bool gameIsRunning = false;
SDL_Window*   window   = NULL;
SDL_Renderer* renderer = NULL;

bool initializeWindow(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL failed to initialize: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    if (!window)
    {
        SDL_Log("Window failed to create: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Renderer failed to create: %s", SDL_GetError());
        return false;
    }

    return true;
}

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            gameIsRunning = false;
            break;
        default:
            break;
        }
    }
}

void update(void)
{
    // empty for now
    // we will add snake logic here later
}

void draw(void)
{
    // dark green background
    SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255);
    SDL_RenderClear(renderer);

    // show frame
    SDL_RenderPresent(renderer);
}

void destroyWindow(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    gameIsRunning = initializeWindow();

    while (gameIsRunning)
    {
        process_input();
        update();
        draw();
        SDL_Delay(16);   // ~60fps
    }

    destroyWindow();
    return 0;
}