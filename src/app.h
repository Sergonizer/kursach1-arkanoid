#pragma once
#include "window.h"
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

class app
{
    std::string path_ = "levels/";
    std::vector<std::string> filepaths;
    int level = 0;
    window window_;
public:
	app()
	{
        if (!window_.init("Arkanoid")) {
            std::cout << "Failed to initialize window" << std::endl;
            return;
        }
        
        for (const auto& entry : fs::directory_iterator(path_))
        {
            filepaths.push_back(entry.path().string());
        }

	}

    void run()
    {
        bool quit = false;
        while (!quit)
        {
            quit = window_.quit_state();

            if ((level < filepaths.size()) && (window_.state() == window::State::NEXT))
            {
                window_.create_field(filepaths[level]);
                level++;
            }

            window_.clear();
            window_.draw(level);
            window_.present();
        }
    }

    ~app()
    {

    }
};