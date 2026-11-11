#include "pch.h"
#include "Scene.h"
#include "Actor.h"
#include "Core/Factory.h"
#include "Renderer/Renderer.h"
#include "Renderer/Light.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"

namespace nu {
	void Scene::AddActor(std::unique_ptr<Actor> actor) {
		actor->m_scene = this;
		m_pendingActors.push_back(std::move(actor));
	}

	void Scene::RemoveAllActors(bool force) {
		std::erase_if(m_actors, [force](auto& actor) { return !actor->GetPersist() || force; });
	}

	bool Scene::Load(const std::string& sceneName) {
		json::document_t document;
		if (json::Load(sceneName, document)) {
			if (JSON_HAS_NAME(document, "actors")) {
				for (auto& actorValue : JSON_GET_NAME(document, "actors").GetArray()) {
					std::string typeName;
					JSON_READ_NAME(actorValue, "type", typeName);

					auto actor = Factory::Instance().Create<Actor>(typeName);
					actor->Read(actorValue);

					bool proto = false;
					JSON_READ(actorValue, proto);

					if (proto) {
						std::string name;
						JSON_READ(actorValue, name);
						Factory::Instance().RegisterProto<Actor>(name, std::move(actor));
					}
					else {
						AddActor(std::move(actor));
					}
				}
			}
		}

		return true;
	}

	void Scene::Update(float dt) {
		for (auto& actor : m_actors) {
			actor->Update(dt);
		}

		//UpdateCollisions();

		for (auto& actor : m_actors) {
			if (actor->m_destroyed) {
				actor->OnDestroy();
			}
		}
		std::erase_if(m_actors, [](auto& actor) { return actor->m_destroyed; });

		for (auto& actor : m_pendingActors) {
			actor->Start();
			m_actors.push_back(std::move(actor));
		}
		m_pendingActors.clear();

		//m_actors.insert(m_actors.end(), m_pendingActors.begin(), m_pendingActors.end());
		//m_pendingActors.clear();
	}

	void Scene::Draw(const Renderer& renderer) {
		FrameUniform frameUniform{};

		auto cam = GetActorByName("Camera");
		if (cam) {
			auto camComp = cam->GetComponent<CameraComponent>();
			if(camComp){
				frameUniform.cameraPosition = cam->GetTransform().position;
				frameUniform.view = camComp->GetView();
				frameUniform.projection = camComp->GetProjection();
				frameUniform.viewProjection = frameUniform.projection * frameUniform.view;

				renderer.SetVertexUniform(UniformSlot::Frame, &frameUniform, sizeof(frameUniform));
				renderer.SetFragmentUniform(UniformSlot::Frame, &frameUniform, sizeof(frameUniform));
			}
		}

		LightUniform lightUniform{};
		lightUniform.ambient = glm::vec3( 0.2f );

		auto lights = GetActorByTag("Light");
		for (auto light : lights)
		{
			auto lightComponent = light->GetComponent<LightComponent>();
			if (lightComponent)
			{
				lightUniform.lights[lightUniform.numLights++] = lightComponent->GetLightData();
			}
		}

		renderer.SetVertexUniform(UniformSlot::Light, &lightUniform, sizeof(lightUniform));
		renderer.SetFragmentUniform(UniformSlot::Light, &lightUniform, sizeof(lightUniform));


		for (const auto& actor : m_actors) {
			if (actor && actor->IsActive()) {
				actor->Draw(renderer);
			}
		}
	}

	//void Scene::UpdateCollisions() {
	//	for (auto& actorA : m_actors) {
	//		for (auto& actorB : m_actors) {
	//			if (actorA == actorB || actorA->m_destroyed || actorB->m_destroyed) continue;

	//			auto colliderA = actorA->GetComponent<ColliderComponent>();
	//			auto colliderB = actorB->GetComponent<ColliderComponent>();

	//			if (!colliderA || !colliderB) continue;

	//			if (colliderA->CheckCol(*colliderB)) {
	//				actorA->OnCollision(actorB.get());
	//				actorB->OnCollision(actorA.get());
	//			}
	//		}
	//	}
	//}
}