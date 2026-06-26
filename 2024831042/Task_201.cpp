#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 540
#define CELL_SIZE     20
#define GRID_COLS     SCREEN_WIDTH/CELL_SIZE
#define GRID_ROWS     SCREEN_HEIGHT/CELL_SIZE

#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3

struct Point {
    int x;
    int y;
};

struct Snake {
    Point body[500];
    int length;
    int direction;
};

bool gameIsRunning = false;
SDL_Window*   window   = NULL;
SDL_Renderer* renderer = NULL;
Snake snake;

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

void initializeSnake(void)
{
    snake.length=3; 
    snake.direction=RIGHT;

    snake.body[0].x=GRID_COLS/2;
    snake.body[0].y=GRID_ROWS/2;

    snake.body[1].x=GRID_COLS/2-1;
    snake.body[1].y=GRID_ROWS/2;

    snake.body[2].x=GRID_COLS/2-2;
    snake.body[2].y=GRID_ROWS/2;
}

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            gameIsRunning=false;
            break;
        default:
            break;
        }
    }
}

void update(void)
{
    // empty for now
}

void drawSnake(void)
{
    for (int i=0; i<snake.length;i++)
    {
        if (i==0)
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else
            SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);

        SDL_Rect cell;
        cell.x= snake.body[i].x *CELL_SIZE+1;
        cell.y=snake.body[i].y *CELL_SIZE+1;
        cell.w=CELL_SIZE-2;
        cell.h=CELL_SIZE-2;

        SDL_RenderFillRect(renderer, &cell);
    }
}

void draw(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255);
    SDL_RenderClear(renderer);

    drawSnake();

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
    initializeSnake();

    while (gameIsRunning)
    {
        process_input();
        update();
        draw();
        SDL_Delay(16);
    }

    destroyWindow();
    return 0;
}