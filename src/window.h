#pragma once
#include <vector>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "ball.h"
#include "platform.h"
#include "block.h"
#include "checkbox.h"

const int rad = 10;
const int balldx = 6;
const int balldy = 8;

class window
{
	const double scale = 0.75;
	SDL_DisplayMode SS;
	SDL_Window* window_ = NULL;
	SDL_Renderer* render_ = NULL;

	SDL_Texture* background_tex_ = NULL;

	std::vector<block> blocks_;
	std::vector<checkbox> checkboxes_;
	ball* ball_ = nullptr;
	platform* plat_ = nullptr;

	bool prev_state_ = false;
	bool easy_mode_;
	int lives_;
	int points_;
	double points_multiplier;

public:
	enum class State {
		BEFORE,
		READY,
		GAME,
		NEXT,
		FAIL,
		DEATH,
		GAME_START, 
		WIN
	};

private:
	State state_ = State::BEFORE;

	int BW;
	int BH;
	int BALL;
	int SW;
	int SH;
	int IBW;

	int rows, cols;

	std::chrono::steady_clock::time_point last_time;
	std::chrono::steady_clock::time_point wait_until;
public:
	void clear()
	{
		SDL_RenderClear(render_);
	}
	void present()
	{
		SDL_RenderPresent(render_);
	}

	bool is_level_loaded()
	{
		return blocks_.size();
	}
	
	bool mouse_input(SDL_Event e)
	{
		int mousex = e.motion.x, mousey = e.motion.y;
		if (state_ == State::BEFORE)
		{
			if ((e.type == SDL_MOUSEBUTTONDOWN) && (prev_state_ == false))
			{
				for (auto& check : checkboxes_)
				{
					if ((mousex >= check.x()) && (mousex <= check.x() + check.size()) && (mousey >= check.y()) && (mousey <= check.y() + check.size()))
						check.change_state();
					prev_state_ = true;
				}
			}
			if ((!(e.type == SDL_MOUSEBUTTONDOWN)) && (prev_state_ == true))
			{
				prev_state_ = false;
			}
		}
		if (state_ == State::GAME)
		{

			plat_->move(mousex <= (SW - IBW - plat_->w() / 2) ? mousex >= (plat_->w() / 2) ? mousex - plat_->w() / 2 : 0 : (SW - IBW - plat_->w()));
		}
		return false;
	}

	bool keyboard_input(SDL_Event e)
	{
		const Uint8* key = SDL_GetKeyboardState(NULL);

		if (state_ == State::BEFORE && (key[SDL_SCANCODE_KP_ENTER] || key[SDL_SCANCODE_RETURN])) {
			easy_mode_ = checkboxes_[static_cast<int>(check_box::Easy_Mode)].filled();
			state_ = State::NEXT;
		}
		if (state_ == State::READY && key[SDL_SCANCODE_SPACE]) {
			state_ = State::GAME_START;
		}
		if (key[SDL_SCANCODE_ESCAPE]) {
			return true;
		}
		return false;
	}

	bool quit_state()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				return true;
			if (e.type == SDL_KEYDOWN)
				return keyboard_input(e);
			if (e.type == SDL_MOUSEMOTION || state_ == State::BEFORE)
				return mouse_input(e);
		}
	}

	State state() const
	{
		return state_;
	}

	void set_state(State new_state, bool force = false)
	{
		state_ = new_state;
		if (force)
		{
		}
	}

	bool init(const char* title)
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			return false;
		}

		SDL_GetDesktopDisplayMode(0, &SS);
		SH = scale * SS.h;
		IBW = SH / 3;
		SW = SH + IBW;
		BH = scale * SH / 15;
		BW = scale * (SW - IBW) / 10;
		BALL = BH / 3;
		lives_ = 3;
		points_ = 0;
		points_multiplier = 1;

		window_ = SDL_CreateWindow(title, SS.w / 2 - SW / 2, SS.h / 2 - SH / 2, SW, SH, SDL_WINDOW_ALLOW_HIGHDPI);
		if (window_ == NULL) { return false; }

		render_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (render_ == NULL) { return false; }

		SDL_Surface* temp_surf = NULL;
		temp_surf = SDL_LoadBMP("res/backgr.bmp");
		background_tex_ = SDL_CreateTextureFromSurface(render_, temp_surf);
		if (!background_tex_) { return false; }

		if (TTF_Init() != 0) {
			std::cout << "Failed to initialize TTF " << std::endl;
			return false;
		}

		checkboxes_.emplace_back(checkbox());

		ball_ = new ball();
		plat_ = new platform();

		return true;
	}

	void init_moveable()
	{
		int pw = (SW - IBW) / 5;
		int ph = pw / 5;
		int px = ((SW - IBW) - pw) / 2;
		int py = 7 * SH / 8 - ph;
		plat_->init(px, py, pw, ph);

		int bx = (SW - IBW) / 2;
		int by = py - BALL;
		ball_->init(bx, by, balldx, balldy, rad);
	}

	void create_field(const std::string& fname)
	{
		std::ifstream ifs(fname);
		if (ifs.is_open() == NULL)
		{
			std::cout << "Failed to open the file " << fname << std::endl;
			return;
		}
		char c;
		int tmpcols = 0;
		rows = 0;
		cols = 0;
		while ((c = ifs.get()) != EOF)
		{
			if (c != '\n')
				tmpcols++;
			else
			{
				rows++;
				if (tmpcols > cols)
					cols = tmpcols;
				tmpcols = 0;
			}
		}
		if (c == EOF)
		{
			rows++;
			if (tmpcols > cols)
				cols = tmpcols;
		}
		ifs.clear();
		ifs.seekg(0);

		int border = ((SW - IBW) - cols * BW) / 2;
		int hit;

		blocks_.clear();

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; (c = ifs.get()) != EOF && j < cols; j++)
			{
				while (c == '\n') { c = ifs.get(); }
				if (c == '.')
					hit = 0;
				else
					hit = c - '0';

				blocks_.emplace_back(block(border + j * BW, i * BH, hit, BW, BH));
			}
		}

		init_moveable();
	}

	SDL_Texture* renderText(const std::string& message, const SDL_Color& color, TTF_Font* font)
	{
		SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
		if (surf == nullptr)
		{
			TTF_CloseFont(font);
			return nullptr;
		}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(render_, surf);
		SDL_FreeSurface(surf);
		return texture;
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

	enum class check_box
	{
		Easy_Mode
	};

	void side_menu(int level)
	{
		SDL_Rect txt_rect;
		std::string lives_text = "Lives: " + std::to_string(lives_);
		std::string level_text = "Level: " + std::to_string(level);
		std::string points_text = "Points: " + std::to_string(points_);

		txt_rect = draw_text(lives_text.c_str(), SW - IBW / 2, 0, 50, xAlign::Center, yAlign::Top);

		txt_rect = draw_text(level_text.c_str(), SW - IBW / 2, txt_rect.y + txt_rect.h, 50, xAlign::Center, yAlign::Top);

		draw_text(points_text.c_str(), SW - IBW / 2, txt_rect.y + txt_rect.h, 50, xAlign::Center, yAlign::Top);
	}

	SDL_Rect& draw_text(const char* text, int x, int y, int size, xAlign xalign = xAlign::Center, yAlign yalign = yAlign::Center)
	{
		TTF_Font* font = TTF_OpenFont("C:/Windows/fonts/arial.ttf", scale * size);
		if (font == nullptr)
		{
			SDL_Rect tmp = { 0, 0, 0, 0 };
			return tmp;
		}
		SDL_Texture* font_tex = renderText(text, { 255, 255, 255, 255 }, font);
		TTF_CloseFont(font);

		int fw, fh;
		SDL_QueryTexture(font_tex, NULL, NULL, &fw, &fh);
		int dx = (SW - IBW) / 2;
		switch (xalign)
		{
		case xAlign::Left:
			dx = x;
			break;
		case xAlign::Center:
			dx = x - fw / 2;
			break;
		case xAlign::Right:
			dx = x - fw;
			break;
		}
		int dy = SH / 2;
		switch (yalign)
		{
		case yAlign::Top:
			dy = y;
			break;
		case yAlign::Center:
			dy = y - fh / 2;
			break;
		case yAlign::Bottom:
			dy = y - fh;
			break;
		}
		SDL_Rect rect = { dx, dy, fw, fh };
		SDL_RenderCopy(render_, font_tex, NULL, &rect);
		SDL_DestroyTexture(font_tex);
		return rect;
	}

	SDL_Point* rect_line_intersection(SDL_Rect rect, SDL_Point point1, SDL_Point point2)
	{
		int start_x = point1.x, start_y = point1.y;
		int end_x = point2.x, end_y = point2.y;

		if (SDL_IntersectRectAndLine(&rect, &start_x, &start_y, &end_x, &end_y))
		{
			SDL_Point intersections[2];
			intersections[0] = { start_x, start_y };
			intersections[1] = { end_x, end_y };
			return intersections;
		}
		else
		{
			return NULL;
		}
	}

	void move()
	{
		auto time_cur = std::chrono::high_resolution_clock().now();
		if (last_time.time_since_epoch().count() == 0)
		{
			last_time = time_cur;
			return;
		}
		auto delta_time = time_cur - last_time;
		double rate = delta_time.count() * 60 / 1e9;
		last_time = time_cur;

		if (ball_->x() <= 0 || ball_->x() + BALL >= (SW - IBW))
			ball_->hit_side();
		if (ball_->y() <= 0)
			ball_->hit_ceil();
		if (ball_->y() + BALL > SH)
		{
			state_ = (lives_-- > 1 ? State::FAIL : State::DEATH);
			wait_until = time_cur + std::chrono::milliseconds(1000);
			return;
		}
		SDL_Point p1 = { ball_->x() + ball_->rad() / 2, ball_->y() + ball_->rad() / 2 };
		SDL_Point p2 = { ball_->x() + ball_->rad() / 2 + ball_->dx(rate), ball_->y() + ball_->rad() / 2 - ball_->dy(rate) };
		
		SDL_Point* intersections = rect_line_intersection(plat_->rect(), p1, p2);
		if (intersections)
			if (intersections[0].y == plat_->rect().y)
				ball_->hit_pad((ball_->x() + ball_->rad() / 2 - plat_->x() - plat_->w() / 2) / plat_->w());

		if (ball_->x() < 0)
			ball_->x(0);
		if (ball_->x() + BALL > (SW - IBW))
			ball_->x((SW - IBW) - BALL);
		if (ball_->y() < 0)
			ball_->y(0);
		if (ball_->y() + BALL > SH)
			ball_->y(SH - BALL);


		if (ball_->y() <= rows * BH)
		{
			int yc = 0, xc = 0;

			for (auto iter = blocks_.begin(); iter != blocks_.end();)
			{
				auto& block = *iter;
				if (block.hp() > 0)
				{
					SDL_Point* intersections = rect_line_intersection(block.rect(), p1, p2);
					if (intersections)
					{
						block.hit(easy_mode_);
						SDL_Rect rect = block.rect();
						if (intersections[0].x == rect.x || intersections[0].x == rect.x + rect.w - 1)
							xc = 1;
						if (intersections[0].y == rect.y || intersections[0].y == rect.y + rect.h - 1)
							yc = 1;

						if (block.hp() == 0)
						{
							points_ += points_multiplier * 100;
							points_multiplier *= 1.1;
						}
						else
						{
							points_ += points_multiplier * 50;
						}
					}
					/*
					if ((ball_->x() + BALL >= block.x()) && (ball_->x() <= block.x() + BW) && (ball_->y() + BALL >= block.y()) && (ball_->y() <= block.y() + BH))
					{
						block.hit(easy_mode_);

						if ((ball_->y() + ball_->dy() > block.y() + BH) || (ball_->y() + ball_->dy() + BALL < block.y()))
							yc = 1;
						if ((ball_->x() - ball_->dx() > block.x() + BW) || (ball_->x() - ball_->dx() + BALL < block.x()))
							xc = 1;

						if (block.hp() == 0)
						{
							points_ += points_multiplier * 100;
							points_multiplier *= 1.1;
						}
						else
						{
							points_ += points_multiplier * 50;
						}
					}
					*/
				}
				if (block.hp() == 0)
					iter = blocks_.erase(iter);
				else
					++iter;
			}
			if (yc)
				ball_->hit_ceil();
			if (xc)
				ball_->hit_side();
		}
		if (blocks_.empty())
		{
			state_ = State::NEXT;
			wait_until = time_cur + std::chrono::milliseconds(1000);
		}

		ball_->move(rate);
	}
	
	void draw(int level)
	{
		if (state_ != State::BEFORE && state_ != State::DEATH)
		{
			side_menu(level);
		}
		switch (state_)
		{
		case State::BEFORE:
		{
			SDL_Rect txt_size = draw_text("Press Enter to start", SW / 2, SH / 2, 100);
			TTF_Font* font = TTF_OpenFont("C:/Windows/fonts/arial.ttf", scale * 50);
			if (font == nullptr)
			{
				return;
			}
			if (std::strcmp(checkboxes_[static_cast<int>(check_box::Easy_Mode)].text().c_str(), "Easy mode: "))
				checkboxes_[static_cast<int>(check_box::Easy_Mode)].init(txt_size.x + txt_size.w / 2, txt_size.y + txt_size.h, scale * 50, "Easy mode: ");
			checkboxes_[static_cast<int>(check_box::Easy_Mode)].display(render_, {255, 255, 255, 255}, font);
			TTF_CloseFont(font);
			return;
		}
		case State::FAIL:
		{
			auto time_cur = std::chrono::high_resolution_clock().now();
			if (time_cur <= wait_until)
			{
				SDL_Rect bg = { 0, 0, (SW - IBW), SH };
				SDL_RenderCopy(render_, background_tex_, NULL, &bg);

				for (auto& block : blocks_)
				{
					block.display(render_);
				}
				ball_->display(render_);

				plat_->display(render_);
			}
			else 
			{
				init_moveable();
				state_ = State::READY;
			}
			points_multiplier = 1;
			break;
		}
		case State::GAME:
		{
			move();

			SDL_Rect bg = { 0, 0, (SW - IBW), SH };
			SDL_RenderCopy(render_, background_tex_, NULL, &bg);

			for (auto& block : blocks_)
			{
				block.display(render_);
			}
			ball_->display(render_);

			plat_->display(render_);
			break;
		}
		case State::NEXT:
		{
			auto time_cur = std::chrono::high_resolution_clock().now();
			if (time_cur <= wait_until)
			{
				draw_text("Next level!", SW - (IBW / 2), SH / 2, 50);
			}
			else
			{
				init_moveable();
				state_ = State::READY;
			}
			break;
		}
		case State::READY:
		{
			SDL_Rect bg = { 0, 0, (SW - IBW), SH };
			SDL_RenderCopy(render_, background_tex_, NULL, &bg);

			for (auto& block : blocks_)
			{
				block.display(render_);
			}
			ball_->display(render_);

			plat_->display(render_);
			draw_text("Press Space to launch the ball!", (SW - IBW) / 2, SH / 2, 50);
			break;
		}
		case State::DEATH:
		{
			SDL_Rect txt_rect = draw_text("You died!", SW / 2, SH / 2, 100, xAlign::Center, yAlign::Center);

			std::string lives_text = "Lives: " + std::to_string(lives_);
			std::string level_text = "Level: " + std::to_string(level);
			std::string points_text = "Points: " + std::to_string(points_);

			txt_rect = draw_text(lives_text.c_str(), SW - IBW / 2, txt_rect.y + txt_rect.h, 50, xAlign::Center, yAlign::Top);

			txt_rect = draw_text(level_text.c_str(), SW - IBW / 2, txt_rect.y + txt_rect.h, 50, xAlign::Center, yAlign::Top);

			draw_text(points_text.c_str(), SW - IBW / 2, txt_rect.y + txt_rect.h, 50, xAlign::Center, yAlign::Top);
			break;
		}
		case State::GAME_START:
		{
			last_time = std::chrono::high_resolution_clock().now();

			state_ = State::GAME;
			break;
		}
		}
	}

	~window()
	{
		SDL_DestroyTexture(background_tex_);
		SDL_DestroyRenderer(render_);
		SDL_DestroyWindow(window_);
		SDL_Quit();
	}
};