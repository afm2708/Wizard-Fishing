
#include "Gameplay/Components/MinigameTargetL.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Casting.h>

MinigameTargetL::MinigameTargetL() :
    IComponent(),
    moveX(0.0),
    moveY(0.0),
    moveSpeedX(0.05),
    moveSpeedY(0.05),
    middleX(),
    middleY(),
    flip(20.0f)

{}

MinigameTargetL::~MinigameTargetL() = default;

void MinigameTargetL::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void MinigameTargetL::Update(float deltaTime)
{
    if (!(MinigameTargetL::pause->isPaused))
    {
        if ((MinigameTargetL::minigame->minigameActive))
        {
            GetGameObject()->SetPostion(glm::vec3(MinigameTargetL::minigame->leftEdge));
            GetGameObject()->SetRotation(glm::vec3(-90, -45, MinigameTargetL::minigame->rotation - 90.0));
            GetGameObject()->SetScale(glm::vec3(0.1f, 0.1, 0.1f));
        }
        else {
            GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -20.0));

        }
 
    }

}

void MinigameTargetL::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedX, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedY, 0.01f, 0.01f);
}

nlohmann::json MinigameTargetL::ToJson() const {
	return {
		{ "move_speedX", (moveSpeedX) },
		{ "move_speedY", (moveSpeedY) },
	};
}

MinigameTargetL::Sptr MinigameTargetL::FromJson(const nlohmann::json& blob) {
    MinigameTargetL::Sptr result = std::make_shared<MinigameTargetL>();
	result->moveSpeedX = (blob["move_speedX"]);
	result->moveSpeedY = (blob["move_speedY"]);
	return result;
}
