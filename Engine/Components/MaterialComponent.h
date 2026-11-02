#pragma once
#include "RenderComponent.h"

namespace nu
{
	class MaterialComponent : public RendererComponent
	{
	public:
		CLASS_PROTO(MaterialComponent)

			void Draw(const Renderer& renderer) override;

	private:
	};
}