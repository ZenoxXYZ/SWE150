#include <SDL2/SDL.h>
#include<stdio.h>
#include <iostream>
using namespace std;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 520

//global variables
bool gameisrunning=false;
SDL_Window *window=NULL;
SDL_Renderer *renderer=NULL;

//circle properties
const int center_x=SCREEN_WIDTH/2;
const int center_y=SCREEN_HEIGHT/2;
int radius=20; //grows over time

bool initializewindow(void)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
         cout << "Error: SDL failed to initialize. SDL Error: " << SDL_GetError();
        return false;
    }
    window=SDL_CreateWindow("Task 102-growing cirlce",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
         SCREEN_WIDTH,
         SCREEN_HEIGHT,
        0);
    if(!window)
    {
        cout << "ERROR: FAILED TO OPEN WINDOW. SDL ERROR: " << SDL_GetError();
        return false;
    }
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        cout << "error. Failed to create renderer,SDL ERROR: "<< SDL_GetError();
        return false;
    }
    return true;
}
void process_input(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
         switch(event.type)
         {
            case SDL_QUIT:
            gameisrunning=false;
            break;
            default :
            break;
        }
    }
}
void update(void)
{
    radius++; // grows radius every frame
    if(radius>=SCREEN_HEIGHT/2)
    {
        radius=20;
    }
}
void draw_circle(int cx,int cy,int r)
{
    int x=r;
    int y=0;
    int err=0;
    while(x>=y)
    {
        SDL_RenderDrawLine(renderer,cx-x,cy+y,cx+x,cy+y);
        SDL_RenderDrawLine(renderer,cx-x,cy-y,cx+x,cy-y);
        SDL_RenderDrawLine(renderer,cx-y,cy+x,cx+y,cy+x);
        SDL_RenderDrawLine(renderer,cx-y,cy-x,cx+y,cy-x);
        y++;
        if(err<=0)
        {
            err+=2*y+1;
        }
        else
        {
            x--;
            err+=2*(y-x)+1;
        }
    }
}
void draw()
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    draw_circle(center_x,center_y,radius);
    SDL_RenderPresent(renderer);
}
void destroyWindow(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int main(int argc, char** argv) //total number of arguments passed,content of those arguments
{
    gameisrunning=initializewindow();
    
    while(gameisrunning)
    {
        process_input;
        draw();
        update();
        SDL_Delay(20);
    }
    destroyWindow();
    return 0;
}