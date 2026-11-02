#pragma once
#include "RenderComponent.h"

namespace nu
{
	class LightComponent : public RendererComponent
	{
	public:
		CLASS_PROTO(LightComponent)

			void Draw(const Renderer& renderer) override;

	private:
	};
}