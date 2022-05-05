#pragma once
#include <SDL2/SDL.h>

class platform
{
    SDL_Rect rect_;

    SDL_Texture* tex_ = nullptr;
    SDL_Surface* surf_ = nullptr;

public:
    int x()
    {
        return rect_.x;
    }

    int y()
    {
        return rect_.y;
    }

    int w()
    {
        return rect_.w;
    }

    int h()
    {
        return rect_.h;
    }

    SDL_Rect rect()
    {
        return rect_;
    }

    void init(int x, int y, int w, int h)
    {
        rect_ = { x, y, w, h };
    }

    platform(int x, int y, int w, int h) : platform()
    {
        init(x, y, w, h);
    }

    platform()
    {
        surf_ = SDL_LoadBMP("res/plat.bmp");
    }

    void move(int newx)
    {
        rect_.x = newx;
    }

    void display(SDL_Renderer* render)
    {
        if (surf_ && !tex_)
            tex_ = SDL_CreateTextureFromSurface(render, surf_);
        if (tex_)
        {
            SDL_RenderCopy(render, tex_, NULL, &rect_);
        }
    }
    ~platform()
    {
        SDL_DestroyTexture(tex_);
    }
};
