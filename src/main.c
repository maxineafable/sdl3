#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <math.h>

typedef struct {
    SDL_Window *window; 
    SDL_Renderer *renderer;
    float player_x, player_y;
    SDL_Texture *player_texture;
    int texture_width;
    int texture_height;
    unsigned int last_time;
    unsigned int current_time;
    float delta_time;
} AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    AppState* state = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (!state) return SDL_APP_FAILURE;

    if (!SDL_CreateWindowAndRenderer("SDL3 Window", 800, 600, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->player_x = 0.f;
    state->player_y = 0.f;

    state->texture_width = 0;
    state->texture_height = 0;

    state->last_time = 0;

    // player texture
    SDL_Surface* loaded_surface = IMG_Load("assets/dog.png");
    if (loaded_surface == NULL) {
        SDL_Log("Couldn't load image: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    state->player_texture = SDL_CreateTextureFromSurface(state->renderer, loaded_surface);
    if (state->player_texture == NULL) {
        SDL_Log("Couldn't load texture: %s", SDL_GetError());

        SDL_DestroySurface(loaded_surface);
        loaded_surface = NULL;
        
        return SDL_APP_FAILURE;
    }

    state->texture_width = loaded_surface->w;
    state->texture_height = loaded_surface->h;

    SDL_DestroySurface(loaded_surface);
    loaded_surface = NULL;

    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState* state = (AppState*)appstate;

    // background
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);
    
    // player movement
    state->last_time = state->current_time;
    state->current_time = SDL_GetTicks();
    state->delta_time = (state->current_time - state->last_time) / 1000.f;
    
    float direction_x = 0.f;
    float direction_y = 0.f;

    const float speed = 200.f;
    const float movement_speed = speed * state->delta_time;
    const bool* keyboard = SDL_GetKeyboardState(NULL);

    if (keyboard[SDL_SCANCODE_W]) direction_y -= 1.f;
    if (keyboard[SDL_SCANCODE_S]) direction_y += 1.f;
    if (keyboard[SDL_SCANCODE_A]) direction_x -= 1.f;
    if (keyboard[SDL_SCANCODE_D]) direction_x += 1.f;

    float length = sqrtf((direction_x * direction_x) + (direction_y * direction_y));

    if (length > 0.f) {
        direction_x /= length;
        direction_y /= length;
    }

    state->player_x += direction_x * movement_speed;
    state->player_y += direction_y * movement_speed;

    // player collision with the screen
    int screen_width = 0;
    int screen_height = 0;

    SDL_GetCurrentRenderOutputSize(state->renderer, &screen_width, &screen_height);

    if (state->player_x < 0.f) {
        state->player_x = 0.f;    
    }

    if (state->player_y < 0.f) {
        state->player_y = 0.f;
    }

    if ((state->player_x + state->texture_width) > screen_width) {
        state->player_x = screen_width - state->texture_width; 
    }

    if ((state->player_y + state->texture_height) > screen_height) {
        state->player_y = screen_height - state->texture_height; 
    }

    // player square with image texture
    SDL_FRect rect = { state->player_x, state->player_y, (float)state->texture_width, (float)state->texture_height };
    SDL_RenderTexture(state->renderer, state->player_texture, NULL, &rect);

    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL) {
        AppState* state = (AppState*)appstate;

        SDL_DestroyTexture(state->player_texture);
        state->player_texture = NULL;

        SDL_free(state);
    }
}

