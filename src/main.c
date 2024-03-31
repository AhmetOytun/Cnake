/* Author: Ahmet Oytun Kurtuldu, made this for fun */
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> /* used for text rendering */
#include <SDL2/SDL_mixer.h> /* used for audio */
#include "constants.h" /* used for constants */

enum MenuState currentMenuState = MAIN_MENU;
SDL_Window* window = NULL; /* used to create the window */
SDL_Renderer* renderer = NULL; /* used to render the game */
TTF_Font* font = NULL; // Font pointer
SDL_Color textColor = {255, 255, 255, 255}; // Text color
Mix_Chunk* apple_sound = NULL; /* used for the apple sound */
Mix_Chunk* game_over_sound = NULL; /* used for the game over sound */
Mix_Music* bg_music = NULL; /* used for the background music */
int game_is_running = FALSE; /* used to check if the game is running */
int last_frame_time = 0; /* used to calculate the delta time */
int score = 0; /* score */

struct segment { /* segment struct */
    float x;
    float y;
    float width;
    float height;
} segment;

struct snake { /* snake struct */
    struct segment segments[MAX_SNAKE_SIZE];
    int num_segments;
    float width;
    float height;
    int direction;
} snake;

struct apple { /* apple struct */
    float x;
    float y;
    float width;
    float height;
} apple;

int initialize_window(void) {
    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        return FALSE;
    }
    font = TTF_OpenFont("fonts/comic.ttf", 24);
    if (!font) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        return FALSE;
    }
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_FOREIGN
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
    SDL_Event event; /* used for processing input */
    SDL_PollEvent(&event); /* polls for events */

    switch(event.type){
        case SDL_QUIT: /* if the user closes the window */
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN: /* if the user presses a key */
            if(event.key.keysym.sym == SDLK_ESCAPE){ /* if the user presses the escape key */
                game_is_running = FALSE;
            }if(event.key.keysym.sym == SDLK_UP && snake.direction != DOWN && snake.direction != UP){ /* if the user presses the up key and its not already moving up or down*/
                snake.direction = UP;
            }if(event.key.keysym.sym == SDLK_DOWN && snake.direction != UP && snake.direction != DOWN){ /* if the user presses the down key and its not already moving up or down */
                snake.direction = DOWN;
            }if(event.key.keysym.sym == SDLK_LEFT && snake.direction != RIGHT && snake.direction != LEFT){ /* if the user presses the left key and its not already moving left or right */
                snake.direction = LEFT;
            }if(event.key.keysym.sym == SDLK_RIGHT && snake.direction != LEFT && snake.direction != RIGHT){ /* if the user presses the right key and its not already moving left or right */
                snake.direction = RIGHT;
            }
            break;
    }
}

void show_main_menu(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); /* black */
    SDL_RenderClear(renderer); /* clears the screen */

    char title_text[50];
    char start_text[50];
    char exit_text[50];
    
    snprintf(title_text, sizeof(title_text), "Snake Game");
    snprintf(start_text, sizeof(start_text), "Press Enter to Start");
    snprintf(exit_text, sizeof(exit_text), "Press Escape to Exit");

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, title_text, textColor);
    SDL_Surface* startSurface = TTF_RenderText_Solid(font, start_text, textColor); 
    SDL_Surface* exitSurface = TTF_RenderText_Solid(font, exit_text, textColor);

    if (!titleSurface || !startSurface || !exitSurface) {
        fprintf(stderr, "Error rendering text: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);

    if (!titleTexture || !startTexture || !exitTexture) {
        fprintf(stderr, "Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(titleSurface);
        return;
    }

    int titleWidth = titleSurface->w;
    int titleHeight = titleSurface->h;

    int startWidth = startSurface->w;
    int startHeight = startSurface->h;

    int exitWidth = exitSurface->w;
    int exitHeight = exitSurface->h;

    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(startSurface);
    SDL_FreeSurface(exitSurface);

    SDL_Rect titleRect = {WINDOW_WIDTH / 2 - titleWidth / 2, WINDOW_HEIGHT / 2 - titleHeight / 2 - 50, titleWidth, titleHeight};
    SDL_Rect startRect = {WINDOW_WIDTH / 2 - startWidth / 2, WINDOW_HEIGHT / 2 - startHeight / 2, startWidth, startHeight};
    SDL_Rect exitRect = {WINDOW_WIDTH / 2 - exitWidth / 2, WINDOW_HEIGHT / 2 - exitHeight / 2 + 50, exitWidth, exitHeight};

    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
    SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);

    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(exitTexture);

    SDL_RenderPresent(renderer); /* renders the screen */

}

void game_over_screen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); /* black */
    SDL_RenderClear(renderer); /* clears the screen */

    char game_over_text[50];
    char score_text[50];
    
    snprintf(game_over_text, sizeof(game_over_text), "Game Over!");
    snprintf(score_text, sizeof(score_text), "Score: %d", score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, game_over_text, textColor);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, score_text, textColor); 
    if (!textSurface || !scoreSurface) {
        fprintf(stderr, "Error rendering text: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    if (!textTexture || !scoreTexture) {
        fprintf(stderr, "Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    int scoreWidth = scoreSurface->w;
    int scoreHeight = scoreSurface->h;

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(scoreSurface);

    SDL_Rect dstRect = {WINDOW_WIDTH / 2 - textWidth / 2, WINDOW_HEIGHT / 2 - textHeight / 2, textWidth, textHeight};
    SDL_Rect scoreRect = {WINDOW_WIDTH / 2 - scoreWidth / 2, WINDOW_HEIGHT / 2 - scoreHeight / 2 + 50, scoreWidth, scoreHeight};

    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer); /* renders the screen */
}

void spawn_apple(){
    apple.x = rand() % (WINDOW_WIDTH - (int)apple.width); /* sets the apple's x position to a random position */
    apple.y = rand() % (WINDOW_HEIGHT - (int)apple.height); /* sets the apple's y position to a random position */
}

void render_score() {
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Score: %d", score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, score_text, textColor);
    if (!textSurface) {
        fprintf(stderr, "Error rendering text: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        fprintf(stderr, "Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect dstRect = {10, 10, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);

    SDL_DestroyTexture(textTexture);
}

void setup(){
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); /* opens the audio */
    apple_sound = Mix_LoadWAV("sounds/score.wav"); /* loads the apple sound */
    game_over_sound = Mix_LoadWAV("sounds/game_over.wav"); /* loads the game over sound */
    bg_music = Mix_LoadMUS("sounds/bg_music.flac"); /* loads the background music */

    Mix_PlayMusic(bg_music, -1); /* plays the background music */

    snake.num_segments = 1;
    snake.segments[0].x = WINDOW_WIDTH / 2 - snake.width / 2;
    snake.segments[0].y = WINDOW_HEIGHT / 2 - snake.height / 2;

    snake.width = 15;
    snake.height = 15;
    snake.direction = NONE; /* sets the snake's direction to right by default */

    apple.height = 15;
    apple.width = 15;
    spawn_apple(); /* spawns the apple */
}

void handle_main_menu_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game_is_running = FALSE;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                game_is_running = FALSE;
            } else if (event.key.keysym.sym == SDLK_RETURN) {
                setup(); /* sets up the game */
                currentMenuState = IN_GAME;
            }
        }
    }
}

void add_segment(){
    if (snake.num_segments < MAX_SNAKE_SIZE) { /* if the snake is not at max size */                    
            snake.segments[snake.num_segments] = snake.segments[snake.num_segments - 1]; /* adds a new segment */
            snake.num_segments++; /* increments the number of segments */
        }
}

int check_for_apple_collision(){ /* checks for apple collision */
        if(snake.segments[0].x < apple.x + apple.width && snake.segments[0].x + snake.width > apple.x && snake.segments[0].y < apple.y + apple.height && snake.segments[0].y + snake.height > apple.y){ /* checks if the snake collides with the apple */
            score++; /* increments the score */
            Mix_PlayChannel(-1, apple_sound, 0); /* plays the apple sound */
            add_segment(); /* adds a new segment */
            spawn_apple(); /* spawns a new apple */
            return TRUE;
        }
    return FALSE;
}

int check_for_wall_collision() { /* checks for wall collision */
    if(snake.segments[0].x < 0 || snake.segments[0].x >= WINDOW_WIDTH || snake.segments[0].y < 0 || snake.segments[0].y >= WINDOW_HEIGHT) {
        return TRUE; 
    }
    return FALSE;
}

int check_for_self_collision() {
    for(int i = 1; i < snake.num_segments; i++) {
        if(snake.segments[0].x == snake.segments[i].x && snake.segments[0].y == snake.segments[i].y) {
            return TRUE;
        }
    }
    return FALSE;
}

void update(){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time); /* calculates the time to wait */
    
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){ /* waits for the time to wait */
        SDL_Delay(time_to_wait);
    }
    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f; /* calculates the delta time */

    last_frame_time = SDL_GetTicks(); /* sets the last frame time */


    for (int i = snake.num_segments - 1; i > 0; i--) { /* updates the snake's segments */
        snake.segments[i].x = snake.segments[i - 1].x;
        snake.segments[i].y = snake.segments[i - 1].y;
    }

    /* update head position */
    switch(snake.direction){ /* moves the snake */
        case RIGHT:
            snake.segments[0].x += SNAKE_SPEED * delta_time;
            break;
        case LEFT:
            snake.segments[0].x -= SNAKE_SPEED * delta_time;
            break;
        case UP:
            snake.segments[0].y -= SNAKE_SPEED * delta_time;
            break;
        case DOWN:
            snake.segments[0].y += SNAKE_SPEED * delta_time;
            break;
    }

    if(check_for_wall_collision() || check_for_self_collision()){ /* checks for wall collision */
        Mix_HaltMusic(); /* stops the background music */
        Mix_HaltChannel(-1); /* stops all sound effects */
        Mix_PlayChannel(-1, game_over_sound, 0); /* plays the game over sound */
        currentMenuState = GAME_OVER;
        return;
    }
    check_for_apple_collision(); /* checks for apple collision */
}

void render(){
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); /* grey */
    SDL_RenderClear(renderer); /* clears the screen */

    for (int i = 0; i < snake.num_segments; i++) {
        SDL_Rect snake_rect = { /* used for drawing the snake */
            (int)snake.segments[i].x,
            (int)snake.segments[i].y,
            (int)snake.width,
            (int)snake.height
        };

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); /* green */
        SDL_RenderFillRect(renderer, &snake_rect); /* makes the snake green */
    }

    SDL_Rect apple_rect = { /* used for drawing the apple */
        (int)apple.x,
        (int)apple.y,
        (int)apple.width,
        (int)apple.height
    };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); /* red */
    SDL_RenderFillRect(renderer, &apple_rect); /* makes the apple red */

    render_score(); 

    SDL_RenderPresent(renderer); /* renders the screen */
}

void destroy_window(){ /* destroys the window and renderer */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (font) {
        TTF_CloseFont(font);
    }
    if (apple_sound) {
        Mix_FreeChunk(apple_sound);
    }
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

void handle_game_over_input(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game_is_running = FALSE;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                game_is_running = FALSE;
            } else if (event.key.keysym.sym == SDLK_RETURN) {
                currentMenuState = MAIN_MENU;
            }
        }
    }
}

int main() {
    game_is_running = initialize_window(); /* initializes the window */

    while(game_is_running){
        switch(currentMenuState){
            case MAIN_MENU:
                show_main_menu(); /* shows the main menu */
                handle_main_menu_input(); /* handles the main menu input */
                break;
            case IN_GAME:
                process_input(); /* processes input */
                update(); /* updates the game */
                render(); /* renders the game */
                break;
            case GAME_OVER:
                game_over_screen(); /* shows the game over screen */
                SDL_Delay(2000); /* waits for 2 seconds */
                score = 0; /* resets the score */
                currentMenuState = MAIN_MENU; /* goes back to the main menu */
                break;
    }
    }

    destroy_window(); /* destroys the window */

    return 0;
}