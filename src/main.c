#include <stdio.h>
#include <SDL2/SDL.h>
#include "constants.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int game_is_running = FALSE;
int last_frame_time = 0;

struct snake {
    float x;
    float y;
    float width;
    float height;
    int direction;
} snake;

int initialize_window(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if(!window){
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT:
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                game_is_running = FALSE;
            }if(event.key.keysym.sym == SDLK_UP && snake.direction != DOWN && snake.direction != UP){
                snake.y -= 10;
                snake.direction = UP;
            }if(event.key.keysym.sym == SDLK_DOWN && snake.direction != UP && snake.direction != DOWN){
                snake.y += 10;
                snake.direction = DOWN;
            }if(event.key.keysym.sym == SDLK_LEFT && snake.direction != RIGHT && snake.direction != LEFT){
                snake.x -= 10;
                snake.direction = LEFT;
            }if(event.key.keysym.sym == SDLK_RIGHT && snake.direction != LEFT && snake.direction != RIGHT){
                snake.x += 10;
                snake.direction = RIGHT;
            }
            break;
    }
}

void setup(){
    snake.x = 0;
    snake.y = 0;
    snake.width = 15;
    snake.height = 15;
    snake.direction = RIGHT;
}

void update(){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    last_frame_time = SDL_GetTicks();

    switch(snake.direction){
        case RIGHT:
            snake.x += SNAKE_SPEED * delta_time;
            break;
        case LEFT:
            snake.x -= SNAKE_SPEED * delta_time;
            break;
        case UP:
            snake.y -= SNAKE_SPEED * delta_time;
            break;
        case DOWN:
            snake.y += SNAKE_SPEED * delta_time;
            break;
    }

}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect snake_rect = {
        (int)snake.x,
        (int)snake.y,
        (int)snake.width,
        (int)snake.height
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &snake_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    game_is_running = initialize_window();

    setup();

    while(game_is_running){
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}