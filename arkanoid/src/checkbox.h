#pragma once
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

class checkbox 
{
	int x_;
	int y_;
	int size_;

	int fw, fh;
	std::string text_;
	bool filled_;

public:
	void init(int x, int y, int size, std::string text, bool filled = false)
	{
		x_ = x;
		y_ = y;
		size_ = size;
		text_ = text;
		filled_ = filled;

	}
	checkbox(int x, int y, int size, std::string text, bool filled = false)
	{
		init(x, y, size, text, filled);
	}
	checkbox()
	{

	}

	std::string text()
	{
		return text_;
	}

	int x()
	{
		return x_ + fw / 2;
	}
	
	int y()
	{
		return y_;
	}

	int size()
	{
		return size_;
	}

	bool filled()
	{
		return filled_;
	}

	enum class xAlign
	{
		Left,
		Center,
		Right
	};

	enum class yAlign
	{
		Top,
		Center,
		Bottom
	};
	
	void change_state()
	{
		filled_ = !filled_;
	}

	void display(SDL_Renderer* render, const SDL_Color& color, TTF_Font* font)
	{
		SDL_Surface* surf = TTF_RenderText_Blended(font, text_.c_str(), color);
		if (surf == nullptr)
		{
			TTF_CloseFont(font);
			return;
		}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surf);
		SDL_FreeSurface(surf);

		SDL_Rect rect;
		SDL_QueryTexture(texture, NULL, NULL, &fw, &fh);
		rect = { x_ - fw / 2, y_, fw, fh };
		SDL_RenderCopy(render, texture, NULL, &rect);

		if (filled())
		{
			SDL_Point tick[3];
			tick[0] = { x_ + fw / 2 + size_ / 4, y_ + size_ / 2 };
			tick[1] = { x_ + fw / 2 + size_ / 3, y_ + 3 * size_ / 4};
			tick[2] = { x_ + fw / 2 + 3 * size_ / 4, y_ + size_ / 4};
			SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderDrawLines(render, tick, 3);
			SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		}

		rect = { x_ + fw / 2, y_, size_, size_ };
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderDrawRect(render, &rect);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_DestroyTexture(texture);
	}
};