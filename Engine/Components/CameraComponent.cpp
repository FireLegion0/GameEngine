#include "pch.h"
#include "CameraComponent.h"
#include "Framework/Actor.h"
#include "Core/Factory.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Start()
	{
		m_aspectRatio = Engine::Get().GetRenderer().GetWidth() / static_cast<float>(Engine::Get().GetRenderer().GetHeight());
	}

	void CameraComponent::Update(float dt)
	{
		glm::vec3 forward = m_owner->GetTransform().Forward();
		glm::vec3 up = m_owner->GetTransform().Up();

		m_view = glm::lookAtRH(m_owner->GetTransform().position, m_owner->GetTransform().position + forward, up);
		m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
	}

	void CameraComponent::Draw(const Renderer& renderer)
	{
	}

	void CameraComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ_NAME(value, "fov", m_fov);
		JSON_READ_NAME(value, "near", m_near);
		JSON_READ_NAME(value, "far", m_far);
	}
}