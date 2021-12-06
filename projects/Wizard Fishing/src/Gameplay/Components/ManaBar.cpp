
#include "Gameplay/Components/ManaBar.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Minigame.h>
#include <Gameplay/Components/SimpleCameraControl.h>
#include <Gameplay/Components/Casting.h>

ManaBar::ManaBar() :
    IComponent(),
    middleX(),
    middleY(),
    middleZ(),
    rotationX(),
    rotationZ()
{}

ManaBar::~ManaBar() = default;

void ManaBar::Awake() {
    _window = GetGameObject()->GetScene()->Window;
}

void ManaBar::Update(float deltaTime)
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

    if (ManaBar::cameraCords->gameStart) {
        GetGameObject()->SetRotation(glm::vec3(-rotationX, 0.0f, rotationZ - 180.0f));
        GetGameObject()->SetPostion(glm::vec3(middleX, middleY, middleZ));
        GetGameObject()->SetScale(glm::vec3(0.00025 * GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->mana, 0.05f, 0.05f));
    }
}

void ManaBar::RenderImGui()
{
}

nlohmann::json ManaBar::ToJson() const {
    return {
    };
}

ManaBar::Sptr ManaBar::FromJson(const nlohmann::json & blob) {
    ManaBar::Sptr result = std::make_shared<ManaBar>();
    return result;
}
