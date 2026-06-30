#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>  
#include <stdio.h>
#include <cstdlib>   // for rand() and srand()
#include <ctime>     // for time()   
#include<string.h> 
#include <string>


//The code uses two standard libraries (<cstdlib> and <ctime>) to calculate dynamic random positions:


//CONSTANTS & GAME GRID CONFIGURATION

#define SCREEN_WIDTH  1080
#define SCREEN_HEIGHT 540
#define CELL_SIZE     20
#define GRID_COLS     SCREEN_WIDTH  / CELL_SIZE   // 1080/20=54 cells wide
#define GRID_ROWS     SCREEN_HEIGHT / CELL_SIZE   // 540/20=27 cells tall

#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3



// DATA STRUCTURES //

struct Point {
    int x;   // column number
    int y;   // row number
};

struct Snake {
    Point body[500];   // snake body positions
    int length;        // how long the snake currently is
    int direction;     // as the direction can't be stored as a text,it is stored as integer. thus up,down,left,right is defined as 0,1,2,3
};

struct Food {
    Point position;    // where the food is on the grid
    bool active;       // is the food currently on screen?
};

//GLOBAL STATE AND SYSTEMS

bool gameIsRunning = false;
bool gameover=false; //tracks whether the snake has died
SDL_Window*   window   = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font     = NULL;

Snake snake;
Food  food;
int score = 0;   // tracks how many food pieces the snake has eaten

Uint32 lastMoveTime = 0;
Uint32 moveDelay    = 80;   //This dictates that the snake will take a step exactly every 80 milliseconds



// SYSTEM INITIALIZATION & CORE FUNCTIONS//


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

    if (TTF_Init() < 0) // initializes the text rendering library,must be called before loading any font
    {
        SDL_Log("TTF failed to initialize: %s", TTF_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Renderer failed to create: %s", SDL_GetError());
        return false;
    }
    font = TTF_OpenFont("PressStart2P-Regular.ttf", 24); // loads the font file at size 24
    if (!font)
    {
        SDL_Log("Font failed to load: %s", TTF_GetError());
        return false;
    }

    return true;
}

void initializeSnake(void) //sets up snake data
{
    snake.length    = 3;
    snake.direction = RIGHT;

    snake.body[0].x = GRID_COLS / 2; // 54/2=27
    snake.body[0].y = GRID_ROWS / 2; // 27/2=13

    snake.body[1].x = GRID_COLS / 2 - 1; // 26
    snake.body[1].y = GRID_ROWS / 2;     // 13

    snake.body[2].x = GRID_COLS / 2 - 2; // 25
    snake.body[2].y = GRID_ROWS / 2;     // 13
}


void spawnFood(void) //places food at a new random position after it gets eaten
{
    bool validPosition = false;

    while (!validPosition) // keeps retrying until food lands on an empty cell
    {
        food.position.x = rand() % GRID_COLS; //Generates a random number and restricts it to a valid tile coordinate (between 0 and 53 for columns, or 0 and 26 for rows) using the modulo operator (%).
        food.position.y = rand() % GRID_ROWS;

        validPosition = true; // assume position is valid unless snake body is found there
        for (int i = 0; i < snake.length; i++)
        {
            if (food.position.x == snake.body[i].x &&
                food.position.y == snake.body[i].y)
            {
                validPosition = false; // If the food coordinates match any part of the snake, validPosition flips back to false.
                break;
            }
        }
    }

    food.active = true;
}
void initializeFood(void) //sets up food data
{
    srand(time(NULL)); // seeds the random number generator using current time so food spawns differently each game

    food.position.x = rand() % GRID_COLS;   // picks a random column between 0 and 53
    food.position.y = rand() % GRID_ROWS;   // picks a random row between 0 and 26
    food.active     = true;                 // food is visible on screen
}
void resetGame(void) //resets everything back to start,called when player presses R.It returns the variables to their exact initial states without clearing memory or destroying your window instance.
{
    gameover = false; 
    score=0;//reset score on restart
    initializeSnake();
    spawnFood(); 
}

// PHYSICS COLLISION CHECKING LOGIC

bool checkwallcollision(void) // returns true if snake head went outside the grid boundaries
{
    int headX=snake.body[0].x;
    int headY=snake.body[0].y;

    if (headX < 0)          return true;  // hit left wall  
    if (headX >= GRID_COLS) return true;  // hit right wall
    if (headY < 0)          return true;  // hit top wall
    if (headY >= GRID_ROWS) return true;  // hit bottom wall


 // if headX falls below 0 (left side) or hits/exceeds GRID_COLS (54 cells total, meaning index 54 is out-of-bounds on the right), it returns true. The exact same safety boundary check applies to the rows (GRID_ROWS).



    return false; // no wall collision
}

bool checkselfcollision(void)
{
    for(int i=1;i<snake.length;i++)
    {
    if(snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y)
    {
        return true;//snake head hit his body
    }
    }
    return false;
}

// INPUT PROCESSING & EVENTS//


void process_input(void) //changes snake direction
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) //this enforces a safety barrier. It ensures you can only turn left or right relative to your current direction, preventing the snake from moving directly backward into itself.
            {
            case SDLK_UP:
                if (snake.direction != DOWN)
                    snake.direction = UP;
                break;
            case SDLK_DOWN:
                if (snake.direction != UP)
                    snake.direction = DOWN;
                break;
            case SDLK_LEFT:
                if (snake.direction != RIGHT)
                    snake.direction = LEFT;
                break;
            case SDLK_RIGHT:
                if (snake.direction != LEFT)
                    snake.direction = RIGHT;
                break;
                case SDLK_r:
    if(gameover)
        resetGame(); // only allow restart if game is actually over
    break;
            }
            break;

        default:
            break;
        }
    }
}

// STATE UPDATES (GAME TICK)//

void update(void)
{
    if(gameover)
    {
        return; // won't update anything if the snake is already dead
    }
    Uint32 currentTime = SDL_GetTicks(); // acts like a stopwatch. It returns how many milliseconds have passed since the game window first opened.
    if (currentTime - lastMoveTime < moveDelay)
        return; //If 150ms haven't passed yet, skip the update and don't move the snake. This keeps the game speed manageable and smooth.
    lastMoveTime = currentTime;

    for (int i = snake.length - 1; i > 0; i--) // shifts every segment back by one so the tail follows the head
    {
        snake.body[i] = snake.body[i - 1];
    }

 // move head forward into a brand new empty grid tile based on current direction
    if (snake.direction == UP)
        snake.body[0].y -= 1;
    else if (snake.direction == DOWN)
        snake.body[0].y += 1;
    else if (snake.direction == LEFT)
        snake.body[0].x -= 1;
    else if (snake.direction == RIGHT)
        snake.body[0].x += 1;


 // check collisions after moving the head

if(checkwallcollision()||checkselfcollision())
{
    gameover=true;//the game is over as a collision has occured
}

// check if snake head is on the same cell as the food
    if (food.active &&
        snake.body[0].x == food.position.x &&
        snake.body[0].y == food.position.y)
    {
        snake.length++;// snake length grows by 1
        score++;//score increases 
        spawnFood();    // place new food somewhere on the grid(random food generation again)
    }
}

void drawSnake(void)
{
    for (int i = 0; i < snake.length; i++) // looping from head to tail
    {
        if (i == 0)
           SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // head is cyan
        else
            SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255); // body is dark green

        SDL_Rect cell; //creates a blank rectangle structure variable named cell
        cell.x = snake.body[i].x * CELL_SIZE + 1; // +1 creates a small visual gap between cells
        cell.y = snake.body[i].y * CELL_SIZE + 1;
        cell.w = CELL_SIZE - 2;   //creates a rectangle that is slightly smaller than the cell boundaries
        cell.h = CELL_SIZE - 2;

        SDL_RenderFillRect(renderer, &cell);
    }
}

void drawFood(void)
{
    if (!food.active) return; // don't draw if there is no food on screen

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // food is red

    SDL_Rect cell; //creates a blank rectangle structure variable named cell
    cell.x = food.position.x * CELL_SIZE + 1;
    cell.y = food.position.y * CELL_SIZE + 1;
    cell.w = CELL_SIZE - 2;
    cell.h = CELL_SIZE - 2;

    SDL_RenderFillRect(renderer, &cell);
}

// GRAPHICS RENDERING ENGINE PIPELINE//


void drawText(const char* text, int x, int y, SDL_Color color) //renders any text string at a given position on screen
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color); // renders text into a pixel surface using the loaded font
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // converts the surface into a GPU texture so SDL can draw it
    SDL_FreeSurface(surface); // surface is no longer needed after converting to texture
    if (!texture) return;

    int textWidth, textHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight); // reads the width and height of the text so we can position it correctly

    SDL_Rect destRect = {x, y, textWidth, textHeight}; // rectangle that tells SDL where and how big to draw the text
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_DestroyTexture(texture); // free texture after drawing,otherwise memory leaks
}
void drawscore(void)
{
    // convert score number to string
    std::string scoreText = "SCORE: " + std::to_string(score);

    // white color for score text
    SDL_Color white = {255, 255, 255, 255};

    // draw at top left corner
    drawText(scoreText.c_str(), 10, 10, white);
}
    void drawgameover(void) //shows a red screen with game over text and final score when the snake dies
{
    SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255); // dark red background
    SDL_RenderClear(renderer);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0,   255};

    drawText("GAME OVER",
             SCREEN_WIDTH /2-150,
             SCREEN_HEIGHT/2-80,
             white);

    std::string finalScore = "SCORE: " + std::to_string(score); // shows the score the player ended with
    drawText(finalScore.c_str(),
             SCREEN_WIDTH  / 2 - 100,
             SCREEN_HEIGHT / 2,
             yellow); // yellow so it stands out from the other text

    drawText("PRESS R TO RESTART",
             SCREEN_WIDTH  / 2 - 220,
             SCREEN_HEIGHT / 2 + 80,
             white);
}
void draw(void)
{
    if(gameover)
    {
        drawgameover();  // show game over screen when snake dies
        SDL_RenderPresent(renderer);
        return;          // skip drawing snake and food
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
    SDL_RenderClear(renderer);

    drawFood();  // draw food first so snake renders on top of it
    drawSnake();
    drawscore();

    SDL_RenderPresent(renderer);
}
// RESOURCE CLEANUP & DESTRUCTION//

void destroyWindow(void) // frees all SDL memory before closing
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    gameIsRunning = initializeWindow();
    initializeSnake();
    initializeFood();

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