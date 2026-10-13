// Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Engine.h"
#include "Core/File.h"
#include "Renderer/Font.h"
#include "Renderer/Text.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TEXT "Hello!\n"
#define MAX(a, b) ((a > b) ? a : b)

using namespace nu;
namespace nu {
    struct Color4
    {
		float r, g, b, a;
    };
}

struct vertexUniform
{
    glm::mat4 model;
};

struct Vertex
{
	Vector3 position;
    Color4 color;
};

std::vector<Vertex> vertices =
{
    Vertex{ Vector3{ -0.7f, -0.5f, 0.0f }, Color4{ 1.0f, 0.0f, 0.0f, 1.0f }}, // Bottom-Left
    Vertex{ Vector3{  0.8f, -0.8f, 0.0f }, Color4{ 0.0f, 1.0f, 0.0f, 1.0f }}, // Bottom-Right
    Vertex{ Vector3{  0.3f,  0.3f, 0.0f }, Color4{ 0.0f, 0.0f, 1.0f, 1.0f }}, // Top-Middle
};

//std::map<std::string, std::unique_ptr<ICreator>> registry;

int main() {
	SetWorkingDirectory("assets"); //Keep this at the top of main() to ensure the working directory is set before any assets are loaded

    //return 0;
    
    // create audio system

    // load the json data from a file
    std::string buffer;

	//INITIALIZATION
    Engine::Get().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Get().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Get().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
	//position
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position));
    //color
    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());


    //SpaceGame game;
    //game.Initialize();

    //float scale = 0.5f;
	Color4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexUniform uniform;

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

        //scale = sin(Engine::Get().GetTime().GetTime());
		float time = Engine::Get().GetTime().GetTime();
        color.r = sin(time * 10.0f) * 0.5f + 0.5f;

        uniform.model = glm::mat4(1.0f);
        uniform.model = glm::translate(uniform.model, glm::vec3(0.5f, 0.0f, 0.0f));
        uniform.model = glm::rotate(uniform.model, time, glm::vec3(0.0f, 0.0f, 1.0f));
        uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        //Engine::Get().GetRenderer().SetVertexUniform(0, &scale, sizeof(scale));
        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Get().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        Engine::Get().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Get().GetRenderer().EndFrame();
    }

    //SHUTDOWN
    Engine::Get().Shutdown();


    return 0;
}
