#include "Gameplay/Components/MorphAnimator.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

MorphAnimator::MorphAnimator()
{ }

MorphAnimator::~MorphAnimator() = default;

void MorphAnimator::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void MorphAnimator::Update(float deltaTime)
{
	
}

void MorphAnimator::RenderImGui()
{

}

nlohmann::json MorphAnimator::ToJson() const {
	return {
	};
}

MorphAnimator::Sptr MorphAnimator::FromJson(const nlohmann::json& blob) {
	MorphAnimator::Sptr result = std::make_shared<MorphAnimator>();
	return result;
}
