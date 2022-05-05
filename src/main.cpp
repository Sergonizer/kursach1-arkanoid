#include "app.h"
#define SDL_MAIN_HANDLED

int main(int argv, char* args[])
{
    app arkanoid;
    arkanoid.run();
    return 0;
}

/*#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <filesystem>

namespace fs = std::experimental::filesystem;
std::string path = "levels/";
std::string *filepaths;
int levels = 0;
int cur = -1;
int win = 1;
int lives = 3;
int rowmax, col;

using namespace std;

const double scale = 0.75;
SDL_DisplayMode SS;
int BW;
int BH;
int BALL;
int SW;
int SH;
int IBW;

SDL_Window* window = NULL;
SDL_Renderer* render = NULL;
SDL_Texture* background = NULL;
SDL_Texture* plat = NULL;
SDL_Texture* bl0 = NULL;
SDL_Texture* bl1 = NULL;
SDL_Texture* bl2 = NULL;
SDL_Texture* bl3 = NULL;
SDL_Texture* bl4 = NULL;
SDL_Texture* bl5 = NULL;
SDL_Texture* ball = NULL;

void draw();
bool quit = false;

enum class State {
    BEFORE,
    READY,
    GAME,
    NEXT,
    FAIL,
    DEATH,
};
State state = State::BEFORE;

SDL_Texture* renderText(const std::string& message, const std::string& fontFile, SDL_Color color, int fontSize, SDL_Renderer* render)
{
    TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (font == nullptr) 
    {
        return nullptr;
    }

    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (surf == nullptr) 
    {
        TTF_CloseFont(font);
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surf);
    SDL_FreeSurface(surf);
    TTF_CloseFont(font);
    return texture;
}

class myblock
{
    int hits;
    int xpos, ypos;

public:
    myblock()
    {
        hits = 0;
        xpos = -1;
        ypos = -1;
    }

    myblock(int xpos_, int ypos_, int hits_)
    {
        xpos = xpos_;
        ypos = ypos_;
        hits = hits_;
    }
    
    void genblock(int xpos_, int ypos_, int hits_)
    {
        xpos = xpos_;
        ypos = ypos_;
        hits = hits_;
    }

    int hp()
    {
        return hits;
    }

    int x()
    {
        return xpos;
    }

    int y()
    {
        return ypos;
    }

    void hit()
    {
        hits--;
    }

    int display()
    {
        SDL_Rect block = { xpos, ypos, BW, BH };

        switch (hits)
        {
        case 0:
            return 1;
            break;
        case 1:
            SDL_RenderCopy(render, bl0, NULL, &block);
            break;
        case 2:
            SDL_RenderCopy(render, bl1, NULL, &block);
            break;
        case 3:
            SDL_RenderCopy(render, bl2, NULL, &block);
            break;
        case 4:
            SDL_RenderCopy(render, bl3, NULL, &block);
            break;
        case 5:
            SDL_RenderCopy(render, bl4, NULL, &block);
            break;
        case 6:
            SDL_RenderCopy(render, bl5, NULL, &block);
            break;
        }
        return 0;
    }

    ~myblock() {}
};

class myblock *blocks;

class myplatform
{
    int pw;
    int ph;

    int px;
    int py;

public:
    int x()
    {
        return px;
    }

    int y()
    {
        return py;
    }

    int w()
    {
        return pw;
    }

    int h()
    {
        return ph;
    }

    void init()
    {
        pw = (SW - IBW) / 5;
        ph = SH / 20;
        px = ((SW - IBW) - pw) / 2;
        py = 7 * SH / 8 - ph;
    }

    void move(int dx)
    {
        px += dx;
    }

    void display()
    {
        SDL_Rect plat_r = { px, py, pw, ph };
        SDL_RenderCopy(render, plat, NULL, &plat_r);
    }
};

class myplatform theplatform;

class myball
{
    double bx;
    double by;
    double dx;
    double dy;
    double speed;

public:
    void init()
    {
        bx = ((SW - IBW) - BALL) / 2;
        by = theplatform.y() - BALL - 1;

        speed = SH / 150; 

        double rng = rand() % 100;
        rng /= 100.0;

        dx = 4 * (rng - 0.5);
        dy = speed - abs(dx);
    }

    void side()
    {
        dx = -dx;
    }

    void ceil()
    {
        dy = -dy;
    }

    void pad()
    {
        ceil();

        if (speed < (SW - IBW) / 75)
        {
            speed *= 1.025;
        }
        dx = -4 * (((theplatform.x() + theplatform.w() / 2 - (bx + BALL / 2))) / theplatform.w());
        dy = (2 * std::signbit(by - theplatform.y()) - 1) * (speed - abs(dx));
    }

    void death()
    {
        dx = 0;
        dy = 0;
        if (lives > 1)
        {
            lives--;
            theplatform.init();
            init();
            std::string txt = "Lives: ";
            txt += std::to_string(lives);
            SDL_Texture* font = renderText(txt, "C:/Windows/Fonts/arial.ttf", { 255, 255, 255 }, 50, render);
            int fw, fh;
            SDL_QueryTexture(font, NULL, NULL, &fw, &fh);
            int x = (SW - IBW) / 2 - fw / 2;
            int y = SH / 2 - fh / 2;
            SDL_Rect text = { x, y, fw, fh };
            SDL_RenderCopy(render, font, NULL, &text);

            SDL_RenderPresent(render);

            SDL_Delay(3000);
        }
        else
        {
            state = State::DEATH;
        }
    }

    void move()
    {
        if (bx <= 0 || bx + BALL >= (SW - IBW))
            side();
        if (by <= 0)
            ceil();
        if (by + BALL > SH)
            death();
        if ((by + BALL >= theplatform.y()) && (by <= theplatform.y() + theplatform.h()) && (bx + BALL >= theplatform.x()) && (bx <= theplatform.x() + theplatform.w()))
            pad();

        if (bx < 0) bx = 0;
        if (bx + BALL > (SW - IBW)) bx = (SW - IBW) - BALL;
        if (by < 0) by = 0;
        if (by + BALL > SH) by = SH - BALL;


        if (by <= col * BH)
        {
            int yc = 0, xc = 0;
            win = 1;
            for (int i = 0; i < col; i++)
            {
                for (int j = 0; j < rowmax; j++)
                {
                    if (blocks[i * rowmax + j].hp() > 0)
                    {
                        if ((bx + BALL >= blocks[i * rowmax + j].x()) && (bx <= blocks[i * rowmax + j].x() + BW) && (by + BALL >= blocks[i * rowmax + j].y()) && (by <= blocks[i * rowmax + j].y() + BH))
                        {
                            blocks[i * rowmax + j].hit();

                            if ((by + dy > blocks[i * rowmax + j].y() + BH) || (by + dy + BALL < blocks[i * rowmax + j].y()))
                                yc = 1;
                            if ((bx - dx > blocks[i * rowmax + j].x() + BW) || (bx - dx + BALL < blocks[i * rowmax + j].x()))
                                xc = 1;
                        }
                    }
                    if (blocks[i * rowmax + j].hp() > 0)
                        win = 0;
                }
            }
            if (yc)
                ceil();
            if (xc)
                side();
        }

        bx += dx;
        by -= dy;
    }

    void display()
    {
        SDL_Rect bal = { bx, by, BALL, BALL };
        SDL_RenderCopy(render, ball, NULL, &bal);
    }
};

myball theball;

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        success = false;
    }

    SDL_GetDesktopDisplayMode(0, &SS);
    SW = scale * SS.w;
    SH = scale * SS.h;
    BW = scale * SH / 10;
    BH = scale * SH / 15;
    BALL = BH / 3;
    IBW = SW - SH;

    window = SDL_CreateWindow("Arkanoid", SS.w / 2 - SW / 2, SS.h / 2 - SH / 2, SW, SH, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) { success = false; }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL) { return false; }

    SDL_Surface* temp_serf = NULL;
    temp_serf = SDL_LoadBMP("backgr.bmp");
    background = SDL_CreateTextureFromSurface(render, temp_serf);
    if (!background) { return false; }

    SDL_Surface* temp_serf2 = NULL;
    temp_serf2 = SDL_LoadBMP("plat.bmp");
    plat = SDL_CreateTextureFromSurface(render, temp_serf2);
    if (!plat) { return false; }

    SDL_Surface* bl = NULL;
    bl = SDL_LoadBMP("block0.bmp");
    bl0 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl0) { return false; }

    bl = SDL_LoadBMP("block1.bmp");
    bl1 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl1) { return false; }

    bl = SDL_LoadBMP("block2.bmp");
    bl2 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl2) { return false; }

    bl = SDL_LoadBMP("block3.bmp");
    bl3 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl3) { return false; }

    bl = SDL_LoadBMP("block4.bmp");
    bl4 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl4) { return false; }

    bl = SDL_LoadBMP("block5.bmp");
    bl5 = SDL_CreateTextureFromSurface(render, bl);
    if (!bl5) { return false; }

    SDL_Surface* bal = NULL;
    bal = SDL_LoadBMP("ball.bmp");
    ball = SDL_CreateTextureFromSurface(render, bal);
    if (!bl5) { return false; }

    return success;
}

void close()
{
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(plat);
    SDL_DestroyTexture(bl0);
    SDL_DestroyTexture(bl1);
    SDL_DestroyTexture(bl2);
    SDL_DestroyTexture(bl3);
    SDL_DestroyTexture(bl4);
    SDL_DestroyTexture(bl5);
    SDL_DestroyTexture(ball);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    background = NULL;
    plat = NULL;
    bl0 = NULL;
    bl1 = NULL;
    bl2 = NULL;
    bl3 = NULL;
    bl4 = NULL;
    bl5 = NULL;
    ball = NULL;
    render = NULL;
    window = NULL;

    SDL_Quit();
}

void gen_field()
{
    ifstream ifs(filepaths[cur]);
    if (ifs.is_open() == NULL)
    {
        cout << "Failed to open the file " << filepaths[cur] << endl;
        return;
    }
    char c;
    int row = 0;
    rowmax = 0;
    col = 0;
    while ((c = ifs.get()) != EOF)
    {
        if (c != '\n')
            row++;
        else
        {
            col++;
            if (row > rowmax)
                rowmax = row;
            row = 0;
        }
    }
    if (c == EOF)
    {
        col++;
        if (row > rowmax)
            rowmax = row;
        row = 0;
    }
    ifs.clear();
    ifs.seekg(0);

    int border = ((SW - IBW) - rowmax * BW) / 2;
    int hit;

    delete[] blocks;
    blocks = new class myblock[col * rowmax];

    for (int i = 0; i < col; i++)
    {
        for (int j = 0; (c = ifs.get()) != EOF && j < rowmax; j++)
        {
            while (c == '\n') { c = ifs.get(); }
            if (c == '.')
                hit = 0;
            else
                hit = c - '0';

            blocks[i*rowmax + j].genblock(border + j * BW, i * BH, hit);
        }
    }

    theplatform.init();
    theball.init();
}

enum class Direction
{
    LEFT,
    RIGHT,
    STOP
};
Direction dir = Direction::STOP;

void draw()
{
    //if (state != State::BEFORE)
    State::FAIL;
    State::GAME;
    State::NEXT;
    State::READY;

    if (state == State::DEATH)
    {
        SDL_Texture* font = renderText("Dead", "C:/Windows/Fonts/arial.ttf", { 255, 255, 255 }, 50, render);
        int fw, fh;
        SDL_QueryTexture(font, NULL, NULL, &fw, &fh);
        int x = (SW - IBW) / 2 - fw / 2;
        int y = SH / 2 - fh / 2;
        SDL_Rect text = { x, y, fw, fh };
        SDL_RenderCopy(render, font, NULL, &text);
    }

    SDL_Rect bg = { 0, 0, (SW - IBW), SH };
    SDL_RenderCopy(render, background, NULL, &bg);

    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < rowmax; j++)
        {
            blocks[i * rowmax + j].display();
        }
    }
    theball.move();
    theball.display();

    if (dir == Direction::LEFT)
    {
        if (theplatform.x() >= 10)
            theplatform.move(-10);
    }
    else if (dir == Direction::RIGHT)
    {
        if (theplatform.x() + theplatform.w() <= (SW - IBW) - 10)
            theplatform.move(10);
    }
    dir = Direction::STOP;
    theplatform.display();
}

void input()
{

    const Uint8* key = SDL_GetKeyboardState(NULL);

    if (key[SDL_SCANCODE_LEFT] && dir != Direction::RIGHT) {
        dir = Direction::LEFT;
    }
    if (key[SDL_SCANCODE_RIGHT] && dir != Direction::LEFT) {
        dir = Direction::RIGHT;
    }
    if (key[SDL_SCANCODE_ESCAPE]) {
        quit = true;
    }
    if (key[SDL_SCANCODE_MINUS]) {
        for (int i = 0; i < col; i++)
        {
            for (int j = 0; j < rowmax; j++)
            {
                if (blocks[i * rowmax + j].hp() > 0)
                    blocks[i * rowmax + j].hit();
            }
        }
    }

}

int main(int argc, char* args[])
{
    srand(time(NULL));

    if (!init()) {
        cout << "Failed to initialize SDL" << endl;
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() != 0) {
        cout << "Failed to initialize TTF " << endl;
        SDL_Quit();
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(path))
    {
        levels++;
        std::cout << entry.path() << std::endl;
    }
    filepaths = new std::string[levels];
    int i = 0;
    for (const auto& entry : fs::directory_iterator(path))
    {
        filepaths[i] = entry.path().string();
        i++;
    }

    SDL_Event e;
    while (!quit)
    {
        if (win)
        {
            cur++;
            win = 0;
            gen_field();
        }

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN)
                input();
        }

        SDL_RenderClear(render);
        draw();
        SDL_RenderPresent(render);
    }

    close();

    return 0;
}*/