#define SDL_MAIN_HANDLED
#include <algorithm>
#include <iostream>
#include <SDL2/SDL.h>

constexpr int WIDTH = 900;
constexpr int HEIGHT = 600;
constexpr int TARGET_FPS = 240;
constexpr int START_RADIUS = 20;
constexpr int COLOR_RECT_SIZE = 30;
constexpr uint32_t START_COLOR = 0xffff00;
constexpr uint32_t CLEAN_COLOR = 0x000000;

uint32_t color_palette[] = {0x000000,0xffffff,0xff0000,0x00ff00,0x0000ff,0x00ffff,0xff00ff,0xffff00};

uint32_t color = START_COLOR;
int brush_size = START_RADIUS;

bool is_pressed = false;

int x_last = 0;
int y_last = 0;

void draw_palette(SDL_Surface* surface, const uint32_t* colors, size_t size)
{
    SDL_Rect color_rect;
    for (size_t i = 0; i < size; ++i)
    {
        color_rect = {static_cast<int>(i*COLOR_RECT_SIZE),0,COLOR_RECT_SIZE,COLOR_RECT_SIZE};
        SDL_FillRect(surface,&color_rect,colors[i]);
    }
}

// check if the mouse is in the section of the color palette.
bool check_position_color_palette(int x, int y)
{
    if (x>=0 && x< static_cast<int>(COLOR_RECT_SIZE*std::size(color_palette)) && y>=0 && y<COLOR_RECT_SIZE)
    {
        return true;
    }
    return false;
}

void draw_circle(SDL_Surface* surface, int x, int y, int radius, uint32_t color)
{
    if (!check_position_color_palette(x,y))
    {
        SDL_Rect pixel = SDL_Rect{0,0,1,1};
        for (int i = y-radius; i < y+radius; ++i)
        {
            for (int j = x-radius; j < x+radius; ++j)
            {
                if (pow((y-i),2)+pow((x-j),2)<=pow(radius,2))
                {
                    pixel.x = j;
                    pixel.y = i;
                    SDL_FillRect(surface,&pixel,color);
                }
            }
        }
    }
}

void check_color_palette_chosen(int x,int y)
{
    if (check_position_color_palette(x,y))
    {
        color = color_palette[x/COLOR_RECT_SIZE];
    }
}

void clean_screen(SDL_Window* window, SDL_Surface* surface,uint32_t clean_color)
{
    SDL_Rect color_rect = {0,0,WIDTH,HEIGHT};
    SDL_FillRect(surface,&color_rect,clean_color);
    SDL_UpdateWindowSurface(window);
    draw_palette(surface,color_palette,std::size(color_palette));
    SDL_UpdateWindowSurface(window);
}

void bresenham(SDL_Surface* surface, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true) {
        draw_circle(surface,x0,y0,brush_size,color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

int main(void)
{
    bool done = false;
    
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window *window = SDL_CreateWindow("ULTRA PAINT",100,100,WIDTH,HEIGHT,0);
    
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    
    float delay_millis = 1000.0f * (1.0f / TARGET_FPS);
    
    bool draw = false;
    int x = 0;
    int y = 0;
    
    draw_palette(surface,color_palette,std::size(color_palette));
    SDL_UpdateWindowSurface(window);
    
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = event.motion.x;
                    y = event.motion.y;
                    check_color_palette_chosen(x,y);
                    draw_circle(surface,x,y,brush_size,color);
                    is_pressed = true;
                    x_last = x;
                    y_last = y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    is_pressed = false;
                    break;
                case SDL_MOUSEMOTION:
                    x = event.motion.x;
                    y = event.motion.y;
                    if (is_pressed)
                    {
                        bresenham(surface,x,y,x_last,y_last);
                    }
                    x_last = x;
                    y_last = y;
                    break;
                case SDL_MOUSEWHEEL:
                    brush_size = std::max(brush_size, 4);
                    brush_size += event.wheel.y;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_TAB)
                    {
                        clean_screen(window,surface,CLEAN_COLOR);
                    }
                    else if (event.key.keysym.sym == SDLK_s)
                    {
                        SDL_SaveBMP(surface, "paint.bmp");
                        printf("The painting has been saved as paint.bmp\n");
                    }
                    break;
                default: ;
            }
        }

        draw_palette(surface,color_palette,std::size(color_palette));
        SDL_UpdateWindowSurface(window);
        
        SDL_Delay(static_cast<uint32_t>(delay_millis));
        
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

