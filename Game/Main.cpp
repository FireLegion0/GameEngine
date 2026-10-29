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

#define TEXT "Hello!\n"
#define MAX(a, b) ((a > b) ? a : b)
#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0);

using namespace nu;

enum class LightType : uint32_t {
    Directional,
    Point,
    Spot
};

namespace nu {
    struct Color4
    {
		float r, g, b, a;
    };
}


struct Vertex
{
	Vector3 position;
    Color4 color;
    Vector2 uv;
};

std::vector<Vertex> vertices =
{
    Vertex{ Vector3{ -0.5f, -0.5f, 0.0f }, Color4{ 1.0f, 0.0f, 0.0f, 1.0f }}, // Bottom-Left
    Vertex{ Vector3{  0.5f, -0.5f, 0.0f }, Color4{ 0.0f, 1.0f, 0.0f, 1.0f }}, // Bottom-Right
    Vertex{ Vector3{  0.5f,  0.5f, 0.0f }, Color4{ 0.0f, 0.0f, 1.0f, 1.0f }}, // Top-Right
    Vertex{ Vector3{ -0.5f,  0.5f, 0.0f }, Color4{ 0.0f, 1.0f, 1.0f, 1.0f }}  // Top-Left
};

std::vector<uint32_t> indices = 
{
    0, 1, 2, 2, 3, 0
};

int main() {
	SetWorkingDirectory("assets"); //Keep this at the top of main() to ensure the working directory is set before any assets are loaded

    //return 0;
    
    // create audio system

    // load the json data from a file
    std::string buffer;

	//INITIALIZATION
    Engine::Get().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(static_cast<uint32_t>(indices.size()), indices.data(), Engine::Get().GetRenderer());

    auto model = Resources().Get<Model>("models/sphere.obj", Engine::Get().GetRenderer());
    auto texture = Resources().Get<Texture>("textures/spot_diffuse.png", Engine::Get().GetRenderer());

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


    //Uniforms
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    struct VertexUniform
    {
        glm::mat4 mvp;
        glm::mat4 model;
        glm::vec3 cameraPos;
        float pad1;
    };
    CHECK_UNIFORM_BOUNDARY(VertexUniform)

    VertexUniform vertexUniform;

    struct LightUniform {
        glm::vec3 lightPos;
        LightType type;

        glm::vec3 lightCol;
        float lightStr;

        glm::vec3 lightDir;
        float inCut;

        glm::vec3 ambient;
        float outCut;

        glm::vec3 pad;
        float range;
    };
    CHECK_UNIFORM_BOUNDARY(LightUniform)

    LightUniform lightUniform{
        .lightPos{ 0, 0, 0 },
        .type{ LightType::Point },
        .lightCol{ 1, 1, 1 },
        .lightStr{ 0.5 },
        .lightDir{ 0, -1, 0 },
        .inCut{ 0.3f },
        .ambient{ 0.1f },
        .outCut{ 2.6f },
        .range{ 40 }
    };

    struct MaterialUniform {
        glm::vec4 color;
        glm::vec2 tiling;
        glm::vec2 offset;
        float shininess;
        glm::vec3 pad;
    };
    CHECK_UNIFORM_BOUNDARY(MaterialUniform)

    MaterialUniform materialUniform{
        .color{ 1, 1, 1, 1 },
        .tiling{ 1.0f, 1.0f },
        .offset{ 0.0f, 0.0f },
        .shininess{ 64.0f }
    };

	vertexUniform.cameraPos = glm::vec3{ 0.0f, 0.0f, 5.0f };

    Transform transform;

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

		float time = Engine::Get().GetTime().GetTime();
        //color.r = sin(time * 10.0f) * 0.5f + 0.5f;

        glm::vec3 direction{ 0 };
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1.0f;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = +1.0f;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = -1.0f;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = +1.0f;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = -1.0f;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = +1.0f;

		vertexUniform.cameraPos += direction * 4.0f * dt;

		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_1)) lightUniform.type = LightType::Directional;
		if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_2)) lightUniform.type = LightType::Point;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_3))
        {
            lightUniform.type = LightType::Spot;
            lightUniform.lightPos = glm::vec3{ 0, 10, 0 };
            lightUniform.inCut = glm::cos(glm::radians(10.0f));
            lightUniform.outCut = glm::cos(glm::radians(15.0f));
        }

        //materialUniform.offset.x = time;
        //materialUniform.tiling = glm::vec2{ 2.0f };

        //TRANSFORMS
        transform.rotation.y = time * 90;

        //VIEW MATRIX
        auto view = glm::lookAtRH(vertexUniform.cameraPos, vertexUniform.cameraPos + glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

		//PROJECTION MATRIX
		float aspectRatio = Engine::Get().GetRenderer().GetWidth() / (float)Engine::Get().GetRenderer().GetHeight();
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.1f, 20.0f);

        //MVP
        vertexUniform.model = transform.GetMatrix();
        vertexUniform.mvp = projection * view * vertexUniform.model;

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Get().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
        Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));

        Engine::Get().GetRenderer().SetFragmentUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Get().GetRenderer().SetFragmentUniform(1, &lightUniform, sizeof(lightUniform));
        Engine::Get().GetRenderer().SetFragmentUniform(2, &materialUniform, sizeof(materialUniform));
        Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

        model->Draw(Engine::Get().GetRenderer());

        Engine::Get().GetRenderer().EndFrame();
    }

    //SHUTDOWN
    Engine::Get().Shutdown();


    return 0;
}