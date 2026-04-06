#include <SDL2/SDL.h> // entire SDL LIBRARY, windows,rendering,pixel drawing everything
#include <stdio.h> // standard input output
#define screen_width 1080
#define screen_height 520 

bool gameisrunning=false;
SDL_Window *window=NULL; // points to nothing,window doesn't exist yet, will be filled when SDL_CreateWindow() is called
SDL_Renderer *renderer=NULL; // will be filled when SDL_CreateRenderer() is called
const int center_x = screen_width/2;// 1080/2 = 540
const int center_y= screen_height/2;  // 520/2  = 260
const int r = 100;

bool initializewindow(void)
{
    if(SDL_INIT_VIDEO<0)
    {
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }
    window=SDL_CreateWindow(
    "Task 101 - Draw Circle",  // argument 1-title(char * type);shown in the title bar of the window
    SDL_WINDOWPOS_UNDEFINED,   // argument 2- X position-let windows decide
    SDL_WINDOWPOS_UNDEFINED,   // argument 3- Y position-let windows decide
    screen_width,              // argument 4 - width=1080
    screen_height,             // argument 5-  height=520
    0                          // argument 6 - '0'=NORMAL WINDOW, "SDL_WINDOW_FULLSCREEN"=fullscreen "SDL_WINDOW_RESIZABLE"=user can resize it
    );

    if(!window)
    {
         printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError()); // if window is NULL failed to create
        return false;
    }
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED); // "-1"= auto pick best graphics driver
                                                                    // SDL_RENDERER_ACCELERATED = USE GPU FOR FAST DRAWING
    if(!renderer)
    {
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }
    return true;
}
    void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) // loop keeps running until all events are processed. Here events mean everything that has happemed since last frame
    {
        switch (event.type) // check what type of event it is. EXAMPLE: 1)Key pressed 2)mouse moved 3)window closed
        {
        case SDL_QUIT: // user clicked the X button to close the window
            gameisrunning = false; // main loop stops and program exits
            break; // stops checking other cases,exit the switch
        default: // any other event
            break; 
        }
    }
}
void draw_circle(int cx,int cy,int r) // midpoint circle algorithm. 
{
    int x=r; // start at rightmost point of the circle
    int y=0;
    int err=0; // decision parameter
    while(x>=y)
    {
        // uses synmetric
         SDL_RenderDrawLine(renderer, cx - x, cy + y, cx + x, cy + y); // fills all pixels between two points x1,y1 and x2,y2
        SDL_RenderDrawLine(renderer, cx - x, cy - y, cx + x, cy - y);              
        SDL_RenderDrawLine(renderer, cx - y, cy + x, cx + y, cy + x); 
        SDL_RenderDrawLine(renderer, cx - y, cy - x, cx + y, cy - x);
        
        y++;// move to next position
         if (err<=0)
        {
            // still inside circle, just move y
            err += 2 * y + 1;
        }
        else
        {
            // gone outside, pull x back
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}
void draw(void)
{
      // Clear screen with black
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
    SDL_RenderClear(renderer);

    // Draw white circle at center
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    draw_circle(center_x,center_y,r);

    // Show on screen
    SDL_RenderPresent(renderer);
}
void destroywindow(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    gameisrunning = initializewindow();//calls initialize window. if it returns true everything worked,false if anything failed

 while(gameisrunning)
    {
        process_input();
        draw();
    }
  destroywindow();
  return 0;
}