#pragma once
#include <SDL2/SDL.h>
#include <string>

class block
{
    int hits_;
    SDL_Rect rect_;
    static const int TEX_SIZE = 6;
    static SDL_Texture* tex_[TEX_SIZE];

public:
    block(int xpos, int ypos, int hits, int w, int h)
    {
        rect_ = { xpos, ypos, w, h };
        hits_ = hits;
    }

    int hp()
    {
        return hits_;
    }

    int x()
    {
        return rect_.x;
    }

    int y()
    {
        return rect_.y;
    }

    SDL_Rect rect()
    {
        return rect_;
    }

    void hit(bool easy_mode)
    {
        hits_ = easy_mode ? 0 : hits_ - 1;
    }

    static bool load_textures(SDL_Renderer* render)
    {
        SDL_Surface* bl = NULL;
        for (int i = 0; i < TEX_SIZE; i++)
        {
            std::string fp = "res/block" + std::to_string(i) + ".bmp";
            bl = SDL_LoadBMP(fp.c_str());
            tex_[i] = SDL_CreateTextureFromSurface(render, bl);
            if (!tex_[i]) { return false; }
        }
    }

    int display(SDL_Renderer *render)
    {
        switch (hits_)
        {
        case 0:
            return 1;
            break;
        default:
            if (hits_ > 0 && hits_ <= TEX_SIZE)
            {
                if (!tex_[hits_ - 1])
                    load_textures(render);
                if (tex_[hits_ - 1])
                    SDL_RenderCopy(render, tex_[hits_ - 1], NULL, &rect_);
            }
            break;
        }
        return 0;
    }

    ~block()
    {
        for (int i = 0; i < TEX_SIZE; i++)
        {
            if (tex_[i])
            {
                SDL_DestroyTexture(tex_[i]);
                tex_[i] = nullptr;
            }
        }
    }
};


