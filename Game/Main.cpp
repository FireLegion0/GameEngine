// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Font.h"
#include "Renderer/Text.h"

#include <fmod.hpp>

#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "Framework/Scene.h"
#include <map>
#include <memory>
#include <random>
#include <fstream>

#define TEXT "Hello!\n"
#define MAX(a, b) ((a > b) ? a : b)

using namespace nu;

std::map<std::string, std::unique_ptr<ICreator>> registry;

int main() {
	SetWorkingDirectory("assets"); //Keep this at the top of main() to ensure the working directory is set before any assets are loaded

    //return 0;
    
    // create audio system

    // load the json data from a file
    std::string buffer;


	//INITIALIZATION
    Engine::Get().Initialize();

    //SpaceGame game;
    //game.Initialize();


    std::vector<Vector2> points;

    //MAIN LOOP
    bool quit = false;
    while (!quit) {

        //UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
				quit = true;
            }
        }

        //ENGINE
        Engine::Get().Update();

        float dt = Engine::Get().GetTime().GetDeltaTime();
        //game.Update(dt);


        //RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetPS().Draw(Engine::Get().GetRenderer());
        
        Engine::Get().GetRenderer().EndFrame(); // Render the screen
    }

    //SHUTDOWN
    Engine::Get().Shutdown();


    return 0;
}
