#pragma once
#include <SDL2/SDL.h>
#include <cmath>

class ball
{
    double dx_;
    double dy_;
    double speed_;
    double max_speed_;

    bool accel_ = false;

    SDL_FRect rect_;
    SDL_Texture* tex_ = nullptr;
    SDL_Surface* surf_ = nullptr;
public:
    void init(float x, float y, double dx, double dy, float rad)
    {
        rect_ = { x, y, rad, rad };
        dx_ = dx;
        dy_ = dy;
        speed_ = sqrt(dx * dx + dy * dy);
        max_speed_ = 2 * speed_;
    }

    ball(float x, float y, double dx, double dy, float rad) : ball()
    {
        init(x, y, dx, dy, rad);
    }

    ball()
    {
        surf_ = SDL_LoadBMP("res/ball.bmp");
    }

    float x() const
    {
        return rect_.x;
    }

    float y() const
    {
        return rect_.y;
    }

    void x(float v)
    {
        rect_.x = v;
    }

    void y(float v)
    {
        rect_.y = v;
    }

    double dx(double rate) const
    {
        return dx_ * rate;
    }

    double dy(double rate) const
    {
        return dy_ * rate;
    }

    float rad() const
    {
        return rect_.h;
    }

    void hit_side()
    {
        dx_ = -dx_;
    }

    void hit_ceil()
    {
        dy_ = -dy_;
        accel_ = true;
    }


    void hit_pad(double hit_pos)
    {
        if (accel_ && speed_ < max_speed_)
        {
            speed_ *= 1.01;
            accel_ = false;
        }
        dx_ = 16 * hit_pos;
        dy_ = std::sqrt(speed_ * speed_ - dx_ * dx_);
    }

    void move(double rate)
    {
        rect_.x += dx_ * rate;
        rect_.y -= dy_ * rate;
    }

    void display(SDL_Renderer* render)
    {
        if (surf_ && !tex_)
            tex_ = SDL_CreateTextureFromSurface(render, surf_);
        if (tex_)
        {
            SDL_RenderCopyF(render, tex_, NULL, &rect_);
        }
    }
    ~ball()
    {
        SDL_DestroyTexture(tex_);
    }
};