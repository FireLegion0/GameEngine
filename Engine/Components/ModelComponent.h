#pragma once
#include "RenderComponent.h"

namespace nu
{
	class ModelComponent : public RendererComponent
	{
	public:
		CLASS_PROTO(ModelComponent)

			void Draw(const Renderer& renderer) override;

	private:
	};
}