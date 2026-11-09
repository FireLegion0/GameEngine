// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Font.h"
#include "Renderer/Text.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Components/CameraComponent.h"
#include "Resources/ResourceManager.h"

#include <fmod.hpp>

#include <iostream>
#include <vector>
#include "SDL3/SDL.h"
#include "Framework/Scene.h"
#include <map>
#include <memory>
#include <random>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#define MAX(a, b) ((a > b) ? a : b)

using namespace nu;

int main() {
	SetWorkingDirectory("assets"); //Keep this at the top of main() to ensure the working directory is set before any assets are loaded

    //return 0;
    
    // create audio system

    // load the json data from a file
    std::string buffer;

	//INITIALIZATION
    Engine::Get().Initialize();

	Scene scene;
	scene.Load("scenes/scene.json");

    auto texture = Resources().Get<Texture>("textures/lava_diffuse.png", Engine::Get().GetRenderer());

    auto vshader = Resources().Get<Shader>("shaders/phong.vert", Engine::Get().GetRenderer(), 3);
    auto fshader = Resources().Get<Shader>("shaders/phong.frag", Engine::Get().GetRenderer(), 3, 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());
    
	auto& renderer = Engine::Get().GetRenderer();

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
		float time = Engine::Get().GetTime().GetTime();

        //SCENE
        scene.Update(dt);


        // RENDER
        renderer.BeginFrame();

        renderer.SetPipeline(*pipeline);

        renderer.SetFragmentTexture(0, *texture);

		scene.Draw(renderer);

        renderer.EndFrame();
    }

    //SHUTDOWN
    Engine::Get().Shutdown();


    return 0;
}