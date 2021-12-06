#include "Gameplay/Components/MorphMeshRenderer.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

MorphMeshRenderer::MorphMeshRenderer()
	
{ }

MorphMeshRenderer::~MorphMeshRenderer() = default;

void MorphMeshRenderer::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void MorphMeshRenderer::Update(float deltaTime)
{
	
}

void MorphMeshRenderer::RenderImGui()
{

}

nlohmann::json MorphMeshRenderer::ToJson() const {
	return {
	};
}

MorphMeshRenderer::Sptr MorphMeshRenderer::FromJson(const nlohmann::json& blob) {
	MorphMeshRenderer::Sptr result = std::make_shared<MorphMeshRenderer>();
	return result;
}
