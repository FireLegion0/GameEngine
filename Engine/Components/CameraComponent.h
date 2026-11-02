#pragma once
#include "RenderComponent.h"

namespace nu
{
	class CameraComponent : public RendererComponent
	{
	public:
		CLASS_PROTO(CameraComponent)

		void Update(float dt) override;
		void Draw(const Renderer& renderer) override;

		void Read(const json::value_t& value) override;



	private:
		glm::mat4x4 m_view;
		glm::mat4x4 m_projection;

		float m_fov{ 70.0f };
		float m_near{ 0.001f };
		float m_far{ 300.0f };
	};
}