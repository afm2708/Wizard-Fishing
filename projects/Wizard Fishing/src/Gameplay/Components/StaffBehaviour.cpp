
#include "Gameplay/Components/StaffBehaviour.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Casting.h>

StaffBehaviour::StaffBehaviour() :
    IComponent(),
    middleX(),
    middleY(),
    rotation()
{}

StaffBehaviour::~StaffBehaviour() = default;

void StaffBehaviour::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void StaffBehaviour::Update(float deltaTime)
{
    //my head + how far from my head + sin theta
    middleX = (cameraCords->GetGameObject()->GetPosition().x) - 1.5f * (sin(((cameraCords->GetGameObject()->GetRotationEuler().z - 40) * 3.141f / 180.0f)));
    //my head + how far from my head + cos theta
    middleY = (cameraCords->GetGameObject()->GetPosition().y) + 1.5f * (cos(((cameraCords->GetGameObject()->GetRotationEuler().z - 40) * 3.141f / 180.0f)));
    //calculates the rotation
    rotation = cameraCords->GetGameObject()->GetRotationEuler().z;

    GetGameObject()->SetRotation(glm::vec3(0, 0, rotation));
    GetGameObject()->SetPostion(glm::vec3(middleX, middleY, 2.0f));
}

void StaffBehaviour::RenderImGui()
{
}

nlohmann::json StaffBehaviour::ToJson() const {
	return {
	};
}

StaffBehaviour::Sptr StaffBehaviour::FromJson(const nlohmann::json& blob) {
	StaffBehaviour::Sptr result = std::make_shared<StaffBehaviour>();
	return result;
}
