#include<assert.h>
#include<math.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include "SDL.h"

struct timespec time2() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return now;
}

long time_diff(struct timespec t1, struct timespec t2) {
    return ((t2.tv_nsec - t1.tv_nsec) / 1000000 + (t2.tv_sec - t1.tv_sec) * 1000);
}

int rand_gen(int min, int max) {
    int difference = max - min;
    int result = rand() / (RAND_MAX / (difference + 1));
    result = result + min;
    if (result > max) { result = max; }
    return result;
}

struct Vec {
    size_t size_element;
    size_t capacity;
    size_t len;
    void* ptr;
};

struct Vec new_vec(size_t size) {
    struct Vec vec;
    void* ptr = malloc(size * 10);

    vec.size_element = size;
    vec.capacity = 10;
    vec.len = 0;
    vec.ptr = ptr;  

    return vec;
}

void clear_vec(struct Vec* vec) {
    vec->len = 0;
}

void vec_free(struct Vec* vec) {
    free(vec->ptr);
}

void vec_realloc(struct Vec* vec, size_t new_capacity, size_t size) {
    void* new_ptr = malloc(new_capacity * size);
    memcpy(new_ptr, vec->ptr, vec->len * vec->size_element);
    free(vec->ptr);
    vec->capacity = new_capacity;
    vec->ptr = new_ptr;
}

void push_vec_ptr(struct Vec* vec, void* value) {
    assert(vec->size_element == sizeof(size_t)); 
    if (vec->capacity == vec->len) {
        vec_realloc(vec, vec->capacity * 2, sizeof(size_t));
    }
    void** destination_ptr = vec->ptr + (vec->len * sizeof(size_t));
    *destination_ptr = value;
    vec->len = vec->len + 1;
}

void push_vec_vec(struct Vec* vec, struct Vec value) {
    assert(vec->size_element == sizeof(struct Vec)); 
    if (vec->capacity == vec->len) {
        vec_realloc(vec, vec->capacity * 2, sizeof(struct Vec));
    }
    struct Vec* destination_ptr = vec->ptr + (vec->len * sizeof(struct Vec));
    *destination_ptr = value;
    vec->len = vec->len + 1;
}

void* get_vec_ptr(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(size_t));
    assert(vec->len > num);
    void** destination_ptr = vec->ptr + (num * sizeof(size_t));
    return *destination_ptr;
}

struct Vec* get_vec_vec(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(struct Vec));
    assert(vec->len > num);
    struct Vec* destination_ptr = vec->ptr + (num * sizeof(struct Vec));
    return destination_ptr;
}

void vec_swap_remove_ptr(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(size_t));
    assert(num < vec->len);
    if (num != vec->len - 1) {
        memcpy(vec->ptr + num * sizeof(size_t), vec->ptr + (vec->len - 1) * sizeof(size_t), sizeof(size_t));
    }
    vec->len = vec->len - 1;
}

void vec_swap_remove_vec(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(struct Vec));
    assert(num < vec->len);
    if (num != vec->len - 1) {
        memcpy(vec->ptr + num * sizeof(struct Vec), vec->ptr + (vec->len - 1) * sizeof(struct Vec), sizeof(struct Vec));
    }
    vec->len = vec->len - 1;
}

struct Point2d {
    int x;
    int y;
};

struct MouseEvents {
    struct Point2d mouse_position;
    struct Point2d mouse_position_matrix;
    bool mouse_left_pressed;
    bool mouse_right_pressed;
    bool mouse_middle_pressed;
};

struct KeyboardEvents {
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool space_pressed;
    bool w_pressed;
    bool r_pressed;
};

struct Events {
    struct MouseEvents mouse_events;
    struct KeyboardEvents keyboard_events;
    bool quit;
};

struct Rectangle {
    int x;
    int y;
};

struct Camera {
    int x;
    int y;
    float z;
};

struct Constants {
    int x;
    int y;
    bool max_camera;
    size_t local_player;
    int matrix_size;
    struct Rectangle size_map;
    float unit_speed;
    int inertia;
    int max_inertia;
    float collision_speed;
    int max_split;
};

struct Window {
    SDL_Window* window;
    SDL_Event input;
    struct Events events;
};

struct Assets {
    struct Vec textures;
    struct Vec sounds;
};

struct DrawingBuffer {
    struct Vec local_cells;
    struct Vec drawable_units;
};

struct Player {
    size_t index;
    struct Vec cells;
    struct Vec buildings;
    struct Vec food;
};

struct Game {
    struct Assets assets;
    struct DrawingBuffer drawing_buffer;
    struct Camera camera;
    struct Vec players;
    struct Vec units;
    struct Vec matrix_unit;
    struct Vec units_killed;
};

struct App {
    struct Window window;
    SDL_Renderer* renderer;
    struct Constants constants;
    struct Game game;
};

struct Texture {
    SDL_Texture* image;
};

struct Texture* initialize_texture(struct App* app, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t pixels[1000][1000][4];

    for (int x = 0; x < 1000; x = x + 1) {
        for (int y = 0; y < 1000; y = y + 1) {
            pixels[x][y][0] = 0;
            pixels[x][y][1] = 0;
            pixels[x][y][2] = 0;
            pixels[x][y][3] = 0;
            float distance = sqrt(((float)x - 500) * ((float)x - 500) + ((float)y - 500) * ((float)y - 500));
            if (distance < 500) {
                pixels[x][y][0] = 255;
                pixels[x][y][1] = 0;
                pixels[x][y][2] = 0;
                pixels[x][y][3] = 0;
                if (distance < 480) {
                    pixels[x][y][0] = 255;
                    pixels[x][y][1] = blue;
                    pixels[x][y][2] = green;
                    pixels[x][y][3] = red;
                }
            }
        }
    }

    SDL_Texture* sdl_texture;
    SDL_Surface* sdl_surface = SDL_CreateRGBSurfaceWithFormatFrom(
        &pixels,
        1000,
        1000,
        1,
        1000 * 4,
        SDL_PIXELFORMAT_RGBA8888
    );

    sdl_texture = SDL_CreateTextureFromSurface(app->renderer, sdl_surface);        
    SDL_FreeSurface(sdl_surface);
    
    struct Texture texture = { .image = sdl_texture };
    struct Texture* texture_ptr = malloc(sizeof(struct Texture));
    *texture_ptr = texture;

    push_vec_ptr(&app->game.assets.textures, texture_ptr);

    return texture_ptr;
}

void initialize_all_textures(struct App* app) {
   initialize_texture(app, 200, 10, 10); //red
   initialize_texture(app, 10, 200, 10); //green
   initialize_texture(app, 10, 10, 200); //blue
   initialize_texture(app, 80, 80, 200); //light blue
   initialize_texture(app, 200, 100, 10); //orange
   initialize_texture(app, 200, 10, 100); //purple
   initialize_texture(app, 200, 200, 10); //yellow
}

void destroy_texture(struct Texture* texture) {
    SDL_Texture* sdl_texture = (*texture).image;
    SDL_DestroyTexture(sdl_texture);
}

struct Texture* return_texture(struct App* app, char* texture) {
    if (strcmp(texture, "Red cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 0); }
    else if (strcmp(texture, "Green cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 1); }
    else if (strcmp(texture, "Blue cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 2); }
    else if (strcmp(texture, "Light Blue cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 3); }
    else if (strcmp(texture, "Orange cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 4); }
    else if (strcmp(texture, "Purple cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 5); }
    else if (strcmp(texture, "Yellow cell") == 0) { return get_vec_ptr(&app->game.assets.textures, 6); }
    else if (strcmp(texture, "Random") == 0) { int rng = rand() / (RAND_MAX / 7); return get_vec_ptr(&app->game.assets.textures, rng); }
    else { assert(false); }
}


struct Player* initialize_player(struct App* app) {
    struct Player player;
    player.index = app->game.players.len;
    player.cells = new_vec(sizeof(size_t));
    player.buildings = new_vec(sizeof(size_t));
    player.food = new_vec(sizeof(size_t));

    struct Player* player_ptr = malloc(sizeof(struct Player));
    *player_ptr = player;

    push_vec_ptr(&app->game.players, player_ptr);
    return player_ptr;
}

enum UnitType {
    Cell,
    Building,
    CellFood,
    Food,
};

struct Unit {
    size_t index;
    struct Player* player;
    enum UnitType unit_type;
    int x;
    int y;
    int64_t mass;
    bool moving;
    int speed_x;
    int speed_y;
    int mergeable_time;
    int collision_time;
    struct Texture* texture;
    struct Unit* killed; //Option
    int64_t matrix_unit_index[3];
    int64_t player_index;
};

struct DrawableUnit {
    int x;
    int y;
    int speed_x;
    int speed_y;
    int64_t mass;
    struct Texture* texture;
};

void push_vec_drawable(struct Vec* vec, struct DrawableUnit value) {
    assert(vec->size_element == sizeof(struct DrawableUnit)); 
    if (vec->capacity == vec->len) {
        vec_realloc(vec, vec->capacity * 2, sizeof(struct DrawableUnit));
    }
    struct DrawableUnit* destination_ptr = vec->ptr + (vec->len * sizeof(struct DrawableUnit));
    *destination_ptr = value;
    vec->len = vec->len + 1;
}

struct DrawableUnit get_vec_drawable(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(struct DrawableUnit));
    struct DrawableUnit* destination_ptr = vec->ptr + (num * sizeof(struct DrawableUnit));
    return *destination_ptr;
}

struct DrawableUnit* get_vec_drawable_ptr(struct Vec* vec, size_t num) {
    assert(vec->size_element == sizeof(struct DrawableUnit));
    struct DrawableUnit* destination_ptr = vec->ptr + (num * sizeof(struct DrawableUnit));
    return destination_ptr;
}

void create_matrix(struct App* app) {
    app->game.matrix_unit = new_vec(sizeof(struct Vec));
    for (int x = 0; x < app->constants.size_map.x; x = x + 1) {
        push_vec_vec(&app->game.matrix_unit, new_vec(sizeof(struct Vec)));
        for (int y = 0; y < app->constants.size_map.y; y = y + 1) {
            push_vec_vec(get_vec_vec(&app->game.matrix_unit, x), new_vec(sizeof(struct Unit*)));
        }
    }
}

void kill_unit_update_matrix(struct App* app, struct Unit* unit) {
    //int64_t* matrix_index = &unit->matrix_unit_index;
    int64_t matrix_index[3];
    matrix_index[0] = unit->matrix_unit_index[0];
    matrix_index[1] = unit->matrix_unit_index[1];
    matrix_index[2] = unit->matrix_unit_index[2];

    vec_swap_remove_ptr(get_vec_vec(get_vec_vec(&app->game.matrix_unit, matrix_index[0]), matrix_index[1]), matrix_index[2]);
    if (matrix_index[2] < get_vec_vec(get_vec_vec(&app->game.matrix_unit, matrix_index[0]), matrix_index[1])->len) {
        struct Unit* unit = get_vec_ptr(get_vec_vec(get_vec_vec(&app->game.matrix_unit, matrix_index[0]), matrix_index[1]), matrix_index[2]);
        unit->matrix_unit_index[2] = matrix_index[2];
    }
}

void add_unit_matrix(struct App* app, struct Unit* unit) {
    int x = unit->x / app->constants.matrix_size;
    int y = unit->y / app->constants.matrix_size;
    unit->matrix_unit_index[0] = x;
    unit->matrix_unit_index[1] = y;
    unit->matrix_unit_index[2] = get_vec_vec(get_vec_vec(&app->game.matrix_unit, x), y)->len;
    push_vec_ptr(get_vec_vec(get_vec_vec(&app->game.matrix_unit, x), y), unit);
}

void move_unit_update_matrix(struct App* app, struct Unit* unit) {
    kill_unit_update_matrix(app, unit);
    add_unit_matrix(app, unit);
}

struct Unit* initialize_unit(struct App* app, struct Player* player, enum UnitType unit_type, int x, int y, int64_t mass, int speed_x, int speed_y, char* texture_str) {
    struct Texture* texture = return_texture(app, texture_str);

    size_t player_index;
    switch (unit_type) {
        case Cell: player_index = player->cells.len; break;
        case Building: player_index = player->buildings.len; break;
        case Food: case CellFood: player_index = player->food.len; break;
    };

    struct Unit unit = {
        .index = app->game.units.len,
        .player= player,
        .unit_type= unit_type,
        .x= x,
        .y= y,
        .mass=mass,
        .moving= true,
        .speed_x= speed_x,
        .speed_y= speed_y,
        .mergeable_time= 0,
        .collision_time= 0,
        .texture= texture,
        .killed= 0,
        .matrix_unit_index= {0,0,0},
        .player_index= player_index,
    };

    struct Unit* unit_ptr = malloc(sizeof(struct Unit));
    *unit_ptr = unit;

    add_unit_matrix(app, unit_ptr);

    switch (unit_ptr->unit_type) {
        case Cell: push_vec_ptr(&player->cells, unit_ptr); break;
        case Building: push_vec_ptr(&player->buildings, unit_ptr); break;
        case Food: case CellFood: push_vec_ptr(&player->food, unit_ptr); break;
    }

    push_vec_ptr(&app->game.units, unit_ptr);

    return unit_ptr;
}

void initialize_cell(struct App* app, struct Player* player, int x, int y, int64_t mass, char* texture) {
    initialize_unit(app, player, Cell, x, y, mass, 0, 0, texture);
}

void initialize_building(struct App* app, struct Player* player, int x, int y, int64_t mass, char* texture) {
    initialize_unit(app, player, Building, x, y, mass, 0, 0, texture);
}

void initialize_food(struct App* app, struct Player* player, int x, int y, int64_t mass, char* texture) {
    initialize_unit(app, player, Food, x, y, mass, 0, 0, texture);
}

void throw_cell(struct App* app, struct Player* player, int x, int y, int direction_x, int direction_y, int initial_speed_x, int initial_speed_y, int64_t mass, char* texture) {
    float moving_x = (direction_x - x);
    float moving_y = (direction_y - y);

    float moving_total = sqrt(moving_x * moving_x + moving_y * moving_y);
    int new_moving_x = 0;
    int new_moving_y = 0;

    float moving = moving_total;
    if (fabs(moving_x) > 1.0) { new_moving_x = (0.5*((sqrt(mass / M_PI)) * moving_x / moving)); }
    if (fabs(moving_y) > 1.0) { new_moving_y = (0.5*((sqrt(mass / M_PI)) * moving_y / moving)); }

    int new_x = x + new_moving_x + initial_speed_x;
    int new_y = y + new_moving_y + initial_speed_y;
    
    if (new_x < 0) { new_x = 0; }
    if (new_x > (app->constants.size_map.x - 1) * app->constants.matrix_size) { new_x = (app->constants.size_map.x - 1) * app->constants.matrix_size; }

    if (new_y < 0) { new_y = 0; }
    if (new_y > (app->constants.size_map.y - 1) * app->constants.matrix_size) { new_y = (app->constants.size_map.y - 1) * app->constants.matrix_size; }

    int mergeable_time = sqrt(mass);
    struct Unit* unit = initialize_unit(app, player, Cell, new_x, new_y, mass, new_moving_x + initial_speed_x, new_moving_y + initial_speed_y, texture);
    unit->mergeable_time = mergeable_time;
    unit->collision_time = 5;
}

void throw_food(struct App* app, struct Player* player, int x, int y, int direction_x, int direction_y, int64_t mass_thrower, int64_t mass, enum UnitType unit_type, float speed, char *texture) {
    float moving_x = (direction_x - x);
    float moving_y = (direction_y - y);

    float moving_total = sqrt(moving_x * moving_x + moving_y * moving_y);
    int new_moving_x = 0;
    int new_moving_y = 0;

    float moving = moving_total;
    if (fabs(moving_x) > 1.0) { new_moving_x = speed*moving_x / moving; }
    if (fabs(moving_y) > 1.0) { new_moving_y = speed*moving_y / moving; }

    float distance = (new_moving_x) * (new_moving_x) + (new_moving_y) * (new_moving_y);
    float delta = (mass_thrower / M_PI) / distance;

    int x_begin = (new_moving_x * sqrt(delta));
    int y_begin = (new_moving_y * sqrt(delta));

    int new_x = x + x_begin + new_moving_x;
    int new_y = y + y_begin + new_moving_y;

    
    if (new_x < 0) { new_x = 0; }
    if (new_x > (app->constants.size_map.x - 1) * app->constants.matrix_size) { new_x = (app->constants.size_map.x - 1) * app->constants.matrix_size; }

    if (new_y < 0) { new_y = 0; }
    if (new_y > (app->constants.size_map.y - 1) * app->constants.matrix_size) { new_y = (app->constants.size_map.y - 1) * app->constants.matrix_size; }
    if (unit_type == Building) {
        initialize_unit(app, player, Food, new_x, new_y, mass, new_moving_x, new_moving_y, texture);
    } else if (unit_type == Cell) {
        initialize_unit(app, player, CellFood, new_x, new_y, mass, new_moving_x, new_moving_y, texture);
    }
}

struct Window new_window() {
    int x = 1280;
    int y = 720;

    SDL_Init(SDL_INIT_EVERYTHING);              

    SDL_Window* window = SDL_CreateWindow(
        "Cell sandbox",
        SDL_WINDOWPOS_UNDEFINED_MASK,
        SDL_WINDOWPOS_UNDEFINED_MASK,
        x,               
        y,       
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED |
        SDL_WINDOW_SHOWN /*| SDL_WindowFlags_SDL_WINDOW_BORDERLESS as u32 */       
    );

    SDL_Event input;
    struct Events events;
    events.keyboard_events.down_pressed = false;
    events.keyboard_events.left_pressed = false;
    events.keyboard_events.r_pressed = false;
    events.keyboard_events.right_pressed = false;
    events.keyboard_events.space_pressed = false;
    events.keyboard_events.up_pressed = false;
    events.keyboard_events.w_pressed = false;

    events.quit = false;
    struct Window w;
    w.events = events;
    w.input = input;
    w.window = window;

    return w;
}

struct App new_app() {
    int x = 1280;
    int y = 720;
    struct Window window = new_window();

    SDL_GetWindowSize(window.window, &x, &y);

    SDL_Renderer* renderer = SDL_CreateRenderer(window.window, -1, 0);

    struct App app;
    app.window = window;
    app.renderer = renderer;

    struct Constants constants;
    constants.x = x;
    constants.y = y;
    constants.max_camera = true;
    constants.local_player = 0;
    constants.matrix_size = 1000;

    struct Rectangle size_map;
    size_map.x = 100;
    size_map.y = 100;

    constants.size_map = size_map;
    constants.unit_speed = 500.0;
    constants.inertia = 10;
    constants.max_inertia = 1500;
    constants.collision_speed = 0.5;
    constants.max_split = 512;

    app.constants = constants;
    
    struct Game game;
    
    struct Assets assets;
    assets.textures = new_vec(sizeof(size_t));
    game.assets = assets;

    struct DrawingBuffer drawing_buffer;
    drawing_buffer.drawable_units = new_vec(sizeof(struct DrawableUnit));
    drawing_buffer.local_cells = new_vec(sizeof(struct DrawableUnit));
    game.drawing_buffer = drawing_buffer;

    struct Camera camera;
    camera.x = 10000;
    camera.y = 10000;
    camera.z = 0.5;
    
    game.camera = camera;
    game.players = new_vec(sizeof(size_t));
    game.units = new_vec(sizeof(size_t));
    game.matrix_unit = new_vec(sizeof(struct Vec));
    game.units_killed = new_vec(sizeof(size_t));

    app.game = game;

    struct Player* computer = initialize_player(&app);
    create_matrix(&app);

    return app;
}


void end_universe(struct App* app) {
    for (int i = 0; i < app->game.units.len; i = i + 1) {
        struct Unit* unit = get_vec_ptr(&app->game.units, i);
        free(unit);
    }
    for (int i = 0; i < app->game.players.len; i = i + 1) {
        struct Player* player = get_vec_ptr(&app->game.players, i);
        free(player);
    }
    for (int i = 0; i < app->game.assets.textures.len; i = i + 1) {
        struct Texture* texture = get_vec_ptr(&app->game.assets.textures, i);
        destroy_texture(texture);
        free(texture);
    }
}

void end_app(struct App* app) {
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window.window);
    SDL_Quit();

    end_universe(app);
}

void synchronize_drawing_buffer(struct App* app) {
    struct DrawingBuffer* buffer = &app->game.drawing_buffer;

    clear_vec(&buffer->drawable_units);

    for (int i = 0; i < app->game.units.len; i = i + 1) {
        struct Unit* unit = get_vec_ptr(&app->game.units, i);
        struct DrawableUnit draw_unit = {
            .x = unit->x,
            .y = unit->y,
            .speed_x = unit->speed_x,
            .speed_y = unit->speed_y,
            .mass = unit->mass,
            .texture = unit->texture,
        };
        push_vec_drawable(&buffer->drawable_units, draw_unit);
    }

    clear_vec(&buffer->local_cells);

    struct Player* local_player = get_vec_ptr(&app->game.players, app->constants.local_player);
    for (int i = 0; i < local_player->cells.len; i = i + 1) {
        struct Unit* unit = get_vec_ptr(&local_player->cells, i);
        struct DrawableUnit draw_unit = {
            .x = unit->x,
            .y = unit->y,
            .speed_x = unit->speed_x,
            .speed_y = unit->speed_y,
            .mass = unit->mass,
            .texture = unit->texture,
        };
        push_vec_drawable(&buffer->local_cells, draw_unit);
    }
}

void poll_event(struct Window* window, SDL_Renderer* renderer, struct Constants* constants, struct Game* game) {
    while (SDL_PollEvent(&window->input)> 0) {
        SDL_Event input = window->input;
        if (input.type == SDL_QUIT) { window->events.quit = true; } // Close app
        if (input.type == SDL_WINDOWEVENT) {
            if (input.window.event == SDL_WINDOWEVENT_RESIZED) {
                int x, y;
                SDL_GetWindowSize(window->window, &x, &y);
                constants->x = x;
                constants->y = y;
            }
        }

        { // All mouse events
            struct MouseEvents* mouse_events = &window->events.mouse_events;

            if (input.type == SDL_MOUSEBUTTONDOWN) { // Mouse button pressed
                if (input.button.button == SDL_BUTTON_LEFT)  { mouse_events->mouse_left_pressed = true; } // Mouse left button
                if (input.button.button == SDL_BUTTON_RIGHT)  { mouse_events->mouse_right_pressed = true; } // Mouse right button
                if (input.button.button == SDL_BUTTON_MIDDLE)  { mouse_events->mouse_middle_pressed = true; } // Mouse middle button
            }

            if (input.type == SDL_MOUSEBUTTONUP) { // Mouse button released
                if (input.button.button == SDL_BUTTON_LEFT)  { mouse_events->mouse_left_pressed = false; } // Mouse left button
                if (input.button.button == SDL_BUTTON_RIGHT)  { mouse_events->mouse_right_pressed = false; } // Mouse right button
            }

            if (input.type == SDL_MOUSEMOTION) { // Mouse moved
                struct Point2d position = { .x = input.button.x , .y = input.button.y };
                mouse_events->mouse_position = position;
            }

            if (input.type == SDL_MOUSEWHEEL) { // Mousewheel
                int wheel_count = window->input.wheel.y;
                if (wheel_count > 0) { constants->max_camera = false; }
                struct Player* local_player = get_vec_ptr(&game->players, constants->local_player);
                game->camera.z = game->camera.z * (1.0 + (wheel_count) * 0.1);
            }
        }

        { // All keyboard events
            struct KeyboardEvents* keyboard_events = &window->events.keyboard_events;

            if (input.type == SDL_KEYDOWN) { // Keyboard button pressed
                if (input.key.keysym.sym == SDLK_z) { keyboard_events->up_pressed = true; } // Z
                if (input.key.keysym.sym == SDLK_s) { keyboard_events->down_pressed = true; } // S
                if (input.key.keysym.sym == SDLK_q) { keyboard_events->left_pressed = true; } // Q
                if (input.key.keysym.sym == SDLK_d) { keyboard_events->right_pressed = true; } // D
                if (input.key.keysym.sym == SDLK_w) { keyboard_events->w_pressed = true; } // W
                if (input.key.keysym.sym == SDLK_r) { keyboard_events->r_pressed = true; } // R
                if (input.key.keysym.sym == SDLK_SPACE) { keyboard_events->space_pressed = true; } // Space
                if (input.key.keysym.sym == SDLK_ESCAPE ) { window->events.quit = true; } // Space
            }

            if (input.type == SDL_KEYUP ) { // Keyboard button released
                if (input.key.keysym.sym == SDLK_z) { keyboard_events->up_pressed = false; } // Z
                if (input.key.keysym.sym == SDLK_s) { keyboard_events->down_pressed = false; } // S
                if (input.key.keysym.sym == SDLK_q) { keyboard_events->left_pressed = false; } // Q
                if (input.key.keysym.sym == SDLK_d) { keyboard_events->right_pressed = false; } // D
                if (input.key.keysym.sym == SDLK_w) { keyboard_events->w_pressed = false; } // W
                if (input.key.keysym.sym == SDLK_r) { keyboard_events->r_pressed = false; } // R
            }
            
        }
    }
}

void add_collision_time(struct Unit* unit, int x) {
    unit->collision_time = unit->collision_time + x;
    if (unit->collision_time < 0) { unit->collision_time = 0; };
}

void add_mergeable_time(struct Unit* unit, int x) {
    unit->mergeable_time = unit->mergeable_time + x;
    if (unit->mergeable_time < 0) { unit->mergeable_time = 0; };
}

void add_position(struct App* app, struct Unit* unit, int x, int y) {
    unit->x = unit->x + x;
    unit->y = unit->y + y;

    if (unit->x < 0) { unit->x = 0; }
    if (unit->x > (app->constants.size_map.x) * app->constants.matrix_size - 1) { unit->x = (app->constants.size_map.x) * app->constants.matrix_size - 1; }

    if (unit->y < 0) { unit->y = 0; }
    if (unit->y > (app->constants.size_map.y) * app->constants.matrix_size - 1) { unit->y = (app->constants.size_map.y) * app->constants.matrix_size - 1; }

    move_unit_update_matrix(app, unit);
}

void add_mass(struct Unit* unit, int64_t mass) {
    unit->mass = unit->mass + mass;

    if (unit->mass < 100000) { unit->mass = 100000; }
    if (unit->mass > 1000000000) { unit->mass = 1000000000; }
}

void add_speed(struct App* app, struct Unit* unit, int x, int y) {
    unit->speed_x = unit->speed_x + x;
    unit->speed_y = unit->speed_y + y;

    if (unit->speed_x < -app->constants.max_inertia) { unit->speed_x = -app->constants.max_inertia; } if (unit->speed_x > app->constants.max_inertia) { unit->speed_x = app->constants.max_inertia; }
    if (unit->speed_y < -app->constants.max_inertia) { unit->speed_y = -app->constants.max_inertia; } if (unit->speed_y > app->constants.max_inertia) { unit->speed_y = app->constants.max_inertia; }
}

void check_events(struct App* app) {
    struct Player* local_player = get_vec_ptr(&app->game.players, app->constants.local_player);

    if (app->window.events.mouse_events.mouse_left_pressed == true) {
    }
    if (app->window.events.mouse_events.mouse_right_pressed == true) {
    }
    if (app->window.events.keyboard_events.up_pressed) {
        for (int i = 0; i < local_player->cells.len; i = i + 1) {
            struct Unit* unit = get_vec_ptr(&local_player->cells, i);
            //unit->mass += 1000000;
        }
    }
    
    
    if (app->window.events.keyboard_events.down_pressed) { }
    if (app->window.events.keyboard_events.left_pressed) { }
    if (app->window.events.keyboard_events.right_pressed) { }

    if (app->window.events.keyboard_events.space_pressed) { 
        app->window.events.keyboard_events.space_pressed = false;
        int len = local_player->cells.len;
        for (int i = 0; i < len; i = i + 1) {
            if (local_player->cells.len < app->constants.max_split) {
                struct Unit* unit = get_vec_ptr(&local_player->cells, i);
                if (unit->mass > 100000 * 2) {
                    unit->mass /= 2;
                    int direction_x = app->window.events.mouse_events.mouse_position_matrix.x;
                    int direction_y = app->window.events.mouse_events.mouse_position_matrix.y;
                    throw_cell(app, local_player, unit->x, unit->y, direction_x, direction_y, unit->speed_x, unit->speed_y, unit->mass, "Green cell");
                }
            }
        }
    }

    if (app->window.events.keyboard_events.w_pressed) {
        for (int i = 0; i < local_player->cells.len; i = i + 1) {
            struct Unit* unit = get_vec_ptr(&local_player->cells, i);
            if (unit->mass > 100000) {
                unit->mass -= 60000;
                int direction_x = app->window.events.mouse_events.mouse_position_matrix.x;
                int direction_y = app->window.events.mouse_events.mouse_position_matrix.y;
                throw_food(app, local_player, unit->x, unit->y, direction_x, direction_y, unit->mass, 40000, Cell, 500.0, "Green cell");
            }
        }

    }

    if (app->window.events.keyboard_events.r_pressed) {
        if (local_player->cells.len == 0) {
            int x = rand() / (RAND_MAX / (app->constants.matrix_size * app->constants.size_map.x - 1000));
            int y = rand() / (RAND_MAX / (app->constants.matrix_size * app->constants.size_map.y - 1000));
            initialize_cell(app, local_player, x, y, 1000, "Green cell");
        }
        /*
        for (int i = 0; i < local_player->cells.len; i = i + 1) {
            struct Unit* unit = get_vec_ptr(&local_player->cells, i);
            unit->mass = 100000;
        }
        */
    }
}

struct Point2d mouse_position_matrix(struct App* app, struct Point2d point) {
    int center_x = (app->constants.x/2);
    int center_y = (app->constants.y/2);
    int x = point.x;
    int y = point.y;
    
    int x_matrix = (app->game.camera.x + (x - center_x) / app->game.camera.z);
    int y_matrix = (app->game.camera.y + (y - center_y) / app->game.camera.z);

    struct Point2d point_ingame = { .x = x_matrix, .y = y_matrix };
    return point_ingame;
}

void moving(struct App* app, struct Unit* unit, int direction_x, int direction_y) {
    app->window.events.mouse_events.mouse_position_matrix = mouse_position_matrix(app, app->window.events.mouse_events.mouse_position);

    float moving_x = (direction_x - unit->x);
    float moving_y = (direction_y - unit->y);

    float moving_total = sqrt(moving_x * moving_x + moving_y * moving_y);

    float distance_before_slow = 200.0;
    float delta_speed = 1.0;
    float difference = (moving_total - distance_before_slow) / distance_before_slow;
    if (difference < 0.0) {
        delta_speed -= fabs(difference);
    }


    float new_moving_x = 0.0;
    float new_moving_y = 0.0;
    float moving = moving_total / (app->constants.unit_speed * (1.0 / (log10(unit->mass))));
    if (fabs(moving_x) > 1.0) { new_moving_x = delta_speed * moving_x / moving; }
    if (fabs(moving_y) > 1.0) { new_moving_y = delta_speed * moving_y / moving; }

    int new_x = unit->x + new_moving_x;
    int new_y = unit->y + new_moving_y;

    // inertia
    int new_direction_x = new_x - unit->x;
    int new_direction_y = new_y - unit->y;
    new_direction_x = (new_direction_x + (app->constants.inertia - 1)*unit->speed_x)/app->constants.inertia;
    new_direction_y = (new_direction_y + (app->constants.inertia - 1)*unit->speed_y)/app->constants.inertia;

    int speed_x = new_direction_x - unit->speed_x;
    int speed_y = new_direction_y - unit->speed_y;

    add_position(app, unit, new_direction_x, new_direction_y);
    add_speed(app, unit, speed_x, speed_y);
}

void move_local_cells(struct App* app) {
    int direction_x = app->window.events.mouse_events.mouse_position_matrix.x;
    int direction_y = app->window.events.mouse_events.mouse_position_matrix.y;
    struct Player* local_player = get_vec_ptr(&app->game.players, app->constants.local_player);
    for (int i = 0; i < local_player->cells.len; i = i + 1) {
        struct Unit* unit = get_vec_ptr(&local_player->cells, i);
        moving(app, unit, direction_x, direction_y);
    }
}

bool add_to_killed_unit(struct App* app, struct Unit* unit, struct Unit* unit_killed) {
    if (unit_killed->killed == 0) {
        unit_killed->killed = unit;
        push_vec_ptr(&app->game.units_killed, unit_killed);
        return true;
    }
    return false;
}

void remove_killed_unit(struct App* app, struct Unit* unit_killed) {
    struct Player* player = unit_killed->player;
    size_t player_index = unit_killed->player_index;

    struct Vec* vector_unit;
    switch (unit_killed->unit_type) {
        case Cell: vector_unit = &player->cells; break;
        case Building: vector_unit = &player->buildings; break;
        case Food: case CellFood: vector_unit = &player->food; break;
    }

    vec_swap_remove_ptr(vector_unit, player_index);
    if (player_index < vector_unit->len) {
        struct Unit* unit = get_vec_ptr(vector_unit, player_index);
        unit->player_index = player_index;
    }

    size_t unit_index = unit_killed->index;
    vec_swap_remove_ptr(&app->game.units, unit_index);
    if (unit_index < app->game.units.len) {
        struct Unit* unit = get_vec_ptr(&app->game.units, unit_index);
        unit->index = unit_index;
    }

    kill_unit_update_matrix(app, unit_killed);

    free(unit_killed);
}

void remove_killed_units(struct App* app) {
    for (int i = 0; i < app->game.units_killed.len; i = i + 1) {
        struct Unit* unit_killed = get_vec_ptr(&app->game.units_killed, i);
        remove_killed_unit(app, unit_killed);
    }
    clear_vec(&app->game.units_killed);
}

float compute_distance_squared(int position1[2], int position2[2]) {
    float result = (float)(position1[0] - position2[0]) * (float)(position1[0] - position2[0]) + (float)(position1[1] - position2[1]) * (float)(position1[1] - position2[1]);
    return result;
}

bool check_unit_scope(struct Unit* unit, int position_unit[2], int64_t mass_unit, float radius_unit, struct Unit* unit_to_target) {
    if (unit->unit_type == Cell) {
        if (unit_to_target->unit_type == Cell) {
            if (unit->player == unit_to_target->player) { 
                if (unit->mergeable_time == 0 && unit_to_target->mergeable_time == 0) {
                    if (unit->mass >= unit_to_target->mass) {
                        if (unit->killed != 0) { return false; }
                        int position_unit_target[2] = { unit_to_target->x, unit_to_target->y };
                        float radius_unit_target = sqrt(unit_to_target->mass / M_PI);
                        float scope = (radius_unit - 0.5 * radius_unit_target) * (radius_unit - 0.5 * radius_unit_target);
                        return (compute_distance_squared(position_unit, position_unit_target) <= scope);
                    }
                }
                else {
                    return false;
                }
            }
        }
    }
    if (mass_unit > unit_to_target->mass * 1.33) {
        int position_unit_target[2] = { unit_to_target->x, unit_to_target->y };
        float radius_unit_target = sqrt(unit_to_target->mass / M_PI);
        float scope = (radius_unit - 0.5 * radius_unit_target) * (radius_unit - 0.5 * radius_unit_target);
        return compute_distance_squared(position_unit, position_unit_target) <= scope;
    }
    else {
        return false;
    }
}

struct Vec check_killable_units(struct App* app, struct Unit* unit) {
    int x_field = unit->matrix_unit_index[0];
    int y_field = unit->matrix_unit_index[1];
    int position_unit[2] = { unit->x, unit->y };
    int64_t mass_unit = unit->mass;
    float radius_unit = sqrt(mass_unit / M_PI);
    int scope_field = 1 + (radius_unit/app->constants.matrix_size);
    struct Vec units_killed = new_vec(sizeof(struct Unit*));

    int x_min;  
    int x_max;
    int y_min;  
    int y_max;

    x_min = x_field - scope_field; if (x_min < 0) { x_min = 0; }
    x_max = x_field + scope_field; if (x_max >= app->constants.size_map.x) { x_max = app->constants.size_map.x - 1; }
    y_min = y_field - scope_field; if (y_min < 0) { y_min = 0; }
    y_max = y_field + scope_field; if (y_max >= app->constants.size_map.y) { y_max = app->constants.size_map.y - 1; }

    for (int x = x_min; x <= x_max; x = x + 1) {
        for (int y = y_min; y <= y_max; y = y + 1) {
            for (int index = 0; index < get_vec_vec(get_vec_vec(&app->game.matrix_unit, x), y)->len; index = index + 1) {
                struct Unit* unit_to_target = get_vec_ptr(get_vec_vec(get_vec_vec(&app->game.matrix_unit, x), y), index);
                if (unit->index == unit_to_target->index) { continue; }
                if (check_unit_scope(unit, position_unit, unit->mass, radius_unit, unit_to_target)) {
                    push_vec_ptr(&units_killed, unit_to_target);
                }
            }
        }
    }

    return units_killed;
}

const struct Point2d NO_POINT = { .x = 2147483647, .y = 2147483647};

struct Point2d check_collision(struct App* app, struct Unit* unit, struct Unit* other) {
    if (unit->mergeable_time == 0 && other->mergeable_time == 0) {  return NO_POINT; }
    float radius_unit = sqrt(unit->mass / M_PI);
    float radius_other = sqrt(other->mass / M_PI);
    int position_unit[2] = { unit->x, unit->y };
    int position_other[2] = { other->x, other->y };
    float distance_squared = compute_distance_squared(position_unit, position_other);
    float distance_to_keep_squared = (radius_unit + radius_other) * (radius_unit + radius_other);
    if (distance_squared < distance_to_keep_squared) {
        int distance_x = position_unit[0] - position_other[0];
        int distance_y = position_unit[1] - position_other[1];
        float distance_total = fabs(distance_x) + fabs(distance_y);
        if (distance_total <= 100) {
            if (distance_x == 0) {
                bool sign = rand_gen(0, 1); if (sign == 0) { sign = -1; }
                distance_x = rand_gen(1,10); distance_x *= sign; distance_x *= 10;
            }
            if (distance_y == 0) {
                bool sign = rand_gen(0, 1); if (sign == 0) { sign = -1; }
                distance_y = rand_gen(1,10); distance_y *= sign; distance_y *= 10;
            }
        }

        float distance_to_add_squared = distance_to_keep_squared - distance_squared;
        float ratio = distance_to_add_squared / distance_squared;
        if (ratio > 5.0) {ratio = 5.0;}
        int new_x = unit->x + (distance_x * ratio * app->constants.collision_speed);
        int new_y = unit->y + (distance_y * ratio * app->constants.collision_speed);

        struct Point2d point = { .x = new_x, .y = new_y };
        return point;
    }
    return NO_POINT;
}

void update_collisions(struct App* app, struct Unit* unit) {
    struct Point2d vec_position[8192];
    int vec_len = 0;
    struct Player* player = get_vec_ptr(&app->game.players, app->constants.local_player);
    assert(player->cells.len < 8192);
    
    for (int i = 0; i < player->cells.len; i = i + 1) {
        if (i == unit->player_index) { continue; }
        struct Unit* other = get_vec_ptr(&player->cells, i);
        if (other->collision_time == 0) {
            struct Point2d point = check_collision(app, unit, other);
            if (point.x != NO_POINT.x && point.y != NO_POINT.y) {
                vec_position[vec_len] = point;
                vec_len = vec_len + 1;
            }
        }
    }
    
    if (vec_len > 0) {
        int x = 0;
        int y = 0;
        for (int i = 0; i < vec_len; i = i + 1) {
            x += vec_position[i].x;
            y += vec_position[i].y;
        }
        x = x / vec_len;
        y = y / vec_len;

        // inertia
        int direction_x = x - unit->x;
        int direction_y = y - unit->y;
        direction_x = (direction_x + (app->constants.inertia - 1)*unit->speed_x)/app->constants.inertia;
        direction_y = (direction_y + (app->constants.inertia - 1)*unit->speed_y)/app->constants.inertia;
        unit->speed_x = direction_x; if (unit->speed_x < -app->constants.max_inertia) { unit->speed_x = -app->constants.max_inertia; } if (unit->speed_x > app->constants.max_inertia) { unit->speed_x = app->constants.max_inertia; }
        unit->speed_y = direction_y; if (unit->speed_y < -app->constants.max_inertia) { unit->speed_y = -app->constants.max_inertia; } if (unit->speed_y > app->constants.max_inertia) { unit->speed_y = app->constants.max_inertia; }

        unit->x += direction_x;
        unit->y += direction_y;
    }

}

void building_throw(struct App* app, struct Unit* unit) {
    struct Player* local_player = get_vec_ptr(&app->game.players, app->constants.local_player);
    int quantity = (sqrt(unit->mass/200000));
    for (int i = 0; i < quantity; i = i + 1) {
        int direction_x = rand_gen(-100000,100000) + unit->x;
        int direction_y = rand_gen(-100000,100000) + unit->y;
        throw_food(app, local_player, unit->x, unit->y, direction_x, direction_y, unit->mass, 10000, Building, rand_gen(50, 100), "Random"); 
        unit->mass -= 5000;
    }
}

void remove_mass_building(struct App* app, struct Unit* unit) {
    if (unit->mass < 1000010) {
        int random_number = rand_gen(0,100);
        if (random_number == 0) {
            building_throw(app, unit);
        }
    } else {
        building_throw(app, unit);
    }

    if (unit->mass < 1000000) { unit->mass = 1000000; }
    if (unit->mass > 10000000000) { unit->mass = 10000000000; }
}

void update_units(struct App* app) {
    for (int i = 0; i < app->game.units.len; i = i + 1) {
        struct Unit* unit = get_vec_ptr(&app->game.units, i);
        switch (unit->unit_type) {
            case Cell:
                if (unit->collision_time == 0) {
                    update_collisions(app, unit);
                }
                add_mergeable_time(unit, -1);
                add_collision_time(unit, -1);
                struct Vec units_killed = check_killable_units(app, unit);
                for (int i = 0; i < units_killed.len; i = i + 1) {
                    struct Unit* unit_killed = get_vec_ptr(&units_killed, i);
                    if (add_to_killed_unit(app, unit, unit_killed)) {
                        unit->mass += unit_killed->mass;
                    }
                }
                add_mass(unit, (-unit->mass * 0.0002));
                break;

            case Food: case CellFood:
                if (unit->moving == true) {
                    int new_x = unit->x + unit->speed_x;
                    int new_y = unit->y + unit->speed_y;

                    int direction_x = new_x - unit->x;
                    int direction_y = new_y - unit->y;

                    int speed_x = ((app->constants.inertia - 1) * unit->speed_x) / (app->constants.inertia);
                    int speed_y = ((app->constants.inertia - 1) * unit->speed_y) / (app->constants.inertia);

                    speed_x = speed_x - unit->speed_x;
                    speed_y = speed_y - unit->speed_y;

                    add_position(app, unit, direction_x, direction_y);
                    add_speed(app, unit, speed_x, speed_y);
                    if (unit->speed_x == 0 && unit->speed_y == 0) {
                        unit->moving = false;
                    }
                }
                break;

            case Building:
            {
                struct Vec units_killed_building = check_killable_units(app, unit);
                for (int i = 0; i < units_killed_building.len; i = i + 1) {
                    struct Unit* unit_killed = get_vec_ptr(&units_killed_building, i);
                    if (add_to_killed_unit(app, unit, unit_killed)) {
                        if (unit_killed->unit_type != Food) {
                            unit->mass += unit_killed->mass;
                        }
                    }
                }
                remove_mass_building(app, unit);
            }
        }
    }
}

void update(struct App* app) {
    check_events(app);
    move_local_cells(app);
    update_units(app);
    remove_killed_units(app);
    synchronize_drawing_buffer(app);
}

void clear_screen(struct App* app) {
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
}

void update_camera(struct App* app) {
    struct DrawingBuffer* buffer = &app->game.drawing_buffer;
    struct Vec* buffer_local_player = &buffer->local_cells;
    struct Camera camera = {.x = 0, .y = 0, .z = 0.0};

    if (buffer_local_player->len == 0) { return; }
    for (int i = 0; i < buffer_local_player->len; i = i + 1) {
        struct DrawableUnit unit = get_vec_drawable(buffer_local_player, i);
        camera.x += unit.x;
        camera.y += unit.y;
    }

    camera.x = camera.x / buffer_local_player->len;
    camera.y = camera.y / buffer_local_player->len;

    app->game.camera.x = camera.x;
    app->game.camera.y = camera.y;

    if (camera.z > 1.0) { camera.z = 1.0; }
    int64_t mass_total = 0;
    for (int i = 0; i < buffer_local_player->len; i = i + 1) {
        struct DrawableUnit unit = get_vec_drawable(buffer_local_player, i);
        mass_total += unit.mass;
    }

    float max_z = 1.0 / sqrt(sqrt((mass_total + 10000000)));
    if (app->constants.max_camera == false) {
        if (app->game.camera.z < max_z) {
            app->constants.max_camera = true; app->game.camera.z = max_z;
        }
    } else {
        app->game.camera.z = max_z;
    }
    
}

void draw_units(struct App* app) {
    struct DrawingBuffer* buffer = &app->game.drawing_buffer;
    
    float center_x = (app->constants.x / 2);
    float center_y = (app->constants.y / 2);

    struct Vec differents_mass_units = new_vec(sizeof(struct DrawableUnit));

    for (int i = 0; i < buffer->drawable_units.len; i = i + 1) {
        struct DrawableUnit unit = get_vec_drawable(&buffer->drawable_units, i);
        if (unit.mass != 10000) { push_vec_drawable(&differents_mass_units, unit); continue; }

        SDL_Texture* sdl_texture = unit.texture->image;

        SDL_Rect rect_position = {
            .x = ( ( unit.x + - app->game.camera.x - (2.0 * sqrt(unit.mass / M_PI)) /2.0 ) * app->game.camera.z + center_x),
            .y = ( ( unit.y + - app->game.camera.y - (2.0 * sqrt(unit.mass / M_PI)) /2.0 ) * app->game.camera.z + center_y),
            .w = (sqrt(unit.mass / M_PI) * 2.0 * app->game.camera.z),
            .h = (sqrt(unit.mass / M_PI) * 2.0 * app->game.camera.z),
        };
        
        SDL_RenderCopy(app->renderer, sdl_texture, 0, &rect_position);
    }

    struct Vec differents_mass_units_sorted = new_vec(sizeof(struct DrawableUnit));
    for (int i = 0; i < differents_mass_units.len; i = i + 1) {
        struct DrawableUnit drawable_unit = get_vec_drawable(&differents_mass_units, i);
        int position = 0;
        for (int j = 0; j < differents_mass_units_sorted.len; j = j + 1) {
            if (get_vec_drawable(&differents_mass_units_sorted, j).mass > drawable_unit.mass) {
                break;
            }
            position = j + 1;
        }

        if (position == differents_mass_units_sorted.len) {
            push_vec_drawable(&differents_mass_units_sorted, drawable_unit);
        } else {
            push_vec_drawable(&differents_mass_units_sorted, get_vec_drawable(&differents_mass_units_sorted, differents_mass_units_sorted.len - 1));
            for (int k = differents_mass_units_sorted.len - 1; k > position; k = k - 1) {
                *get_vec_drawable_ptr(&differents_mass_units_sorted, k) = get_vec_drawable(&differents_mass_units_sorted, k - 1);
            }
            *get_vec_drawable_ptr(&differents_mass_units_sorted, position) = drawable_unit;
        }
    }

    assert(differents_mass_units.len == differents_mass_units_sorted.len);

    for (int i = 0; i < differents_mass_units_sorted.len; i = i + 1) {
        struct DrawableUnit unit = get_vec_drawable(&differents_mass_units_sorted, i);
        SDL_Texture* sdl_texture = unit.texture->image;

        SDL_Rect rect_position = {
            .x = ( ( unit.x + - app->game.camera.x - (2.0 * sqrt(unit.mass / M_PI)) /2.0 ) * app->game.camera.z + center_x),
            .y = ( ( unit.y + - app->game.camera.y - (2.0 * sqrt(unit.mass / M_PI)) /2.0 ) * app->game.camera.z + center_y),
            .w = (sqrt(unit.mass / M_PI) * 2.0 * app->game.camera.z),
            .h = (sqrt(unit.mass / M_PI) * 2.0 * app->game.camera.z),
        };

        SDL_RenderCopy(app->renderer, sdl_texture, 0, &rect_position);
    }

    vec_free(&differents_mass_units);
    vec_free(&differents_mass_units_sorted);
}

void draw(struct App* app) {
    clear_screen(app);
    update_camera(app);
    draw_units(app);
    SDL_RenderPresent(app->renderer);
}

int main() {
    srand(time(0));
    struct App app = new_app();


    initialize_all_textures(&app);
    app.constants.local_player = initialize_player(&app)->index;
    struct Player* local_player = get_vec_ptr(&app.game.players, app.constants.local_player);

    initialize_cell(&app, local_player, 50000, 50000, 100000, "Green cell");

    struct timespec now;
    timespec_get(&now, TIME_UTC);

    while (app.window.events.quit == false) {
        while (true) {
            struct timespec new_now;
            timespec_get(&new_now, TIME_UTC);
            if (new_now.tv_nsec - now.tv_nsec > 10000000 || new_now.tv_sec - now.tv_sec > 0) {
                break;
            }
        }
        timespec_get(&now, TIME_UTC);

        poll_event(&app.window, app.renderer, &app.constants, &app.game);

        for(int i = 0; i < 1000; i = i + 1) {
            if (app.game.units.len > 100000) { break; }
            struct Player* computer = get_vec_ptr(&app.game.players, 0);
            for(int i = 0; i < 10; i = i + 1) { 
                int x = rand_gen(0, app.constants.matrix_size * app.constants.size_map.x - 1);
                int y = rand_gen(0, app.constants.matrix_size * app.constants.size_map.y - 1);
                initialize_food(&app, computer, x, y, 10000, "Random");
            }
            if (rand() / (RAND_MAX / 100) == 0) {
                int x = rand_gen(0, app.constants.matrix_size * app.constants.size_map.x - 1);
                int y = rand_gen(0, app.constants.matrix_size * app.constants.size_map.y - 1);
                initialize_building(&app, computer, x, y, 10000, "Red cell");
            }
        }

        update(&app);
        draw(&app);
    }
    end_app(&app);
}