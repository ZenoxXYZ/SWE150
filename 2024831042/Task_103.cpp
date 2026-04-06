#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>
using namespace std;

#define screen_width 1080
#define screen_height 540

bool gameisrunning=false;
SDL_Window *window=NULL;
SDL_Renderer *renderer=NULL;

double CIRCLE1X=0; // circle 1 x co-ordinate
int CIRCLE1Y=screen_height/2; // y coordinate. starts from the middle of the screen from the left
int circle1radius=50;

double CIRCLE2Y=0; // circle 2 y co-ordinate
int CIRCLE2X=screen_width/2; // x coordinate. starts from the middle of the screen from the top
int circle1radius=50;
int circle2radius=50;

bool collision =false;

bool initializewindow(void)
{
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        cout << "ERROR, SDL FAILED TO INITIALIZE. SDL ERROR: "<< SDL_GetError();
        return false;
    }
    window=SDL_CreateWindow("TASK 103_DETECT COLLISION"
    ,SDL_WINDOWPOS_UNDEFINED,
     SDL_WINDOWPOS_UNDEFINED,
     screen_width,
     screen_height,0);
    if(!window)
    {
        cout << "ERROR. FAILED TO INITIALIZE WINDOW,SDL ERROR: "<< SDL_GetError();
        return false;
    }
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        cout << "Error. Failed to create renderer: "<< SDL_GetError();
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
            case SDL_KEYDOWN:
            switch(event.key.keysym.sym) // keyboard keys
            {
                case SDLK_UP: 
                CIRCLE2Y-=5;
                break;
                case SDLK_DOWN: 
                CIRCLE2Y+=5;
                break;
                case SDLK_LEFT: 
                CIRCLE2X-=5;
                break;
                case SDLK_RIGHT: 
                CIRCLE2X+=5;
                break;
            }
            break;
            default:
            break;
        }
    }
}

void update(void)
{
    CIRCLE1X+=2; // move circle 1 right everytime
    if(CIRCLE1X>screen_width)
    {
        CIRCLE1X=0;
    }
    double dx=CIRCLE2X-CIRCLE1X; //x2-x1 ->horizontal distance
    double dy=CIRCLE2Y-CIRCLE1Y; //y2-y1 ->vertical distance
    double distance=sqrt(dx*dx + dy*dy); // distance between the two centers of the cicrlce
    if(distance<circle1radius+circle2radius)
    {
        collision=true;     // if distance between two centers of the circles is less than the sum of radius,then the circles collide
    }
    else
    {
        collision=false;
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
void draw(void)
{
    SDL_SetRenderDrawColor(renderer,0,255,0,0);
    SDL_RenderClear(renderer);

    if(collision)
    {
        SDL_SetRenderDrawColor(renderer,255,0,0,255); // collision detected,color changed to red
    }
    else
    {
        SDL_SetRenderDrawColor(renderer,255,255,255,255); // no collision color stays white
    }
    draw_circle(CIRCLE1X,(int)CIRCLE1Y,circle1radius);
    draw_circle(CIRCLE2X,(int)CIRCLE2Y,circle2radius);
    SDL_RenderPresent(renderer);
}
void destroywindow(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int main(int argc,char **argv)
{
    gameisrunning=initializewindow();

    while(gameisrunning)
    {
        process_input();
        update();
        draw();
        SDL_Delay(16);
    }
    destroywindow();
    return 0;
}