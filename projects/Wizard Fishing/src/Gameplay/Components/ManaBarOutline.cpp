
#include "Gameplay/Components/ManaBarOutline.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Casting.h>

ManaBarOutline::ManaBarOutline() :
    IComponent(),
    middleX(),
    middleY(),
    middleZ(),
    rotationX(),
    rotationZ()
{}

ManaBarOutline::~ManaBarOutline() = default;

void ManaBarOutline::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void ManaBarOutline::Update(float deltaTime)
{

    //calculates the rotation on the X axis
    rotationX = cameraCords->GetGameObject()->GetRotationEuler().x;
    //calculates the rotation on the Z axis
    rotationZ = cameraCords->GetGameObject()->GetRotationEuler().z;

    //my head + how far from my head + sin theta
    middleX = (cameraCords->GetGameObject()->GetPosition().x) - 1.5f * (sin(((cameraCords->GetGameObject()->GetRotationEuler().z) * 3.141f / 180.0f)));
    //my head + how far from my head + cos theta
    middleY = (cameraCords->GetGameObject()->GetPosition().y) + 1.5f * (cos(((cameraCords->GetGameObject()->GetRotationEuler().z) * 3.141f / 180.0f)));

    //my head + how far from my head + cos theta
    middleZ = (cameraCords->GetGameObject()->GetPosition().z) - 1.5f * (cos(((cameraCords->GetGameObject()->GetRotationEuler().x + 40) * 3.141f / 180.0f)));

    GetGameObject()->SetRotation(glm::vec3(-rotationX, 0, rotationZ -180.0f));
    GetGameObject()->SetPostion(glm::vec3(middleX, middleY, middleZ));
}

void ManaBarOutline::RenderImGui()
{
}

nlohmann::json ManaBarOutline::ToJson() const {
	return {
	};
}

ManaBarOutline::Sptr ManaBarOutline::FromJson(const nlohmann::json& blob) {
    ManaBarOutline::Sptr result = std::make_shared<ManaBarOutline>();
	return result;
}
