#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

typedef struct {
    SDL_Window *window; 
    SDL_Renderer *renderer;
    float player_x, player_y;
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

    state->last_time = 0;

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

    const float speed = 100.f;
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

    // player square
    SDL_FRect rect = { state->player_x, state->player_y, 100.f, 100.f };
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->renderer, &rect);

    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_free(appstate);
}

