#include <stdio.h>
#include <SDL2/SDL.h>
#include "constants.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int game_is_running = FALSE;
int last_frame_time = 0;

struct ball {
    float x;
    float y;
    float width;
    float height;
    int direction;
} ball;

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
            }if(event.key.keysym.sym == SDLK_UP && ball.direction != DOWN && ball.direction != UP){
                ball.y -= 10;
                ball.direction = UP;
            }if(event.key.keysym.sym == SDLK_DOWN && ball.direction != UP && ball.direction != DOWN){
                ball.y += 10;
                ball.direction = DOWN;
            }if(event.key.keysym.sym == SDLK_LEFT && ball.direction != RIGHT && ball.direction != LEFT){
                ball.x -= 10;
                ball.direction = LEFT;
            }if(event.key.keysym.sym == SDLK_RIGHT && ball.direction != LEFT && ball.direction != RIGHT){
                ball.x += 10;
                ball.direction = RIGHT;
            }
            break;
    }
}

void setup(){
    ball.x = 0;
    ball.y = 0;
    ball.width = 15;
    ball.height = 15;
    ball.direction = RIGHT;
}

void update(){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    last_frame_time = SDL_GetTicks();

    switch(ball.direction){
        case RIGHT:
            ball.x += BALL_SPEED * delta_time;
            break;
        case LEFT:
            ball.x -= BALL_SPEED * delta_time;
            break;
        case UP:
            ball.y -= BALL_SPEED * delta_time;
            break;
        case DOWN:
            ball.y += BALL_SPEED * delta_time;
            break;
    }

}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

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