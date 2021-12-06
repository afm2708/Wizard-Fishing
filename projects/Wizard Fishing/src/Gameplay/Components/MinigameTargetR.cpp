
#include "Gameplay/Components/MinigameTargetR.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Casting.h>

MinigameTargetR::MinigameTargetR() :
    IComponent(),
    moveX(0.0),
    moveY(0.0),
    moveSpeedX(0.05),
    moveSpeedY(0.05),
    middleX(),
    middleY(),
    flip(20.0f)

{}

MinigameTargetR::~MinigameTargetR() = default;

void MinigameTargetR::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void MinigameTargetR::Update(float deltaTime)
{
    if (!(MinigameTargetR::pause->isPaused))
    {
        if ((MinigameTargetR::minigame->minigameActive))
        {
            GetGameObject()->SetPostion(glm::vec3(MinigameTargetR::minigame->rightEdge));
            GetGameObject()->SetRotation(glm::vec3(90, 45, MinigameTargetR::minigame->rotation + 90.0));
            GetGameObject()->SetScale(glm::vec3(0.1f, 0.1, 0.1f));
        }
        else {
            GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -20.0));

        }
 
    }

}

void MinigameTargetR::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedX, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedY, 0.01f, 0.01f);
}

nlohmann::json MinigameTargetR::ToJson() const {
	return {
		{ "move_speedX", (moveSpeedX) },
		{ "move_speedY", (moveSpeedY) },
	};
}

MinigameTargetR::Sptr MinigameTargetR::FromJson(const nlohmann::json& blob) {
    MinigameTargetR::Sptr result = std::make_shared<MinigameTargetR>();
	result->moveSpeedX = (blob["move_speedX"]);
	result->moveSpeedY = (blob["move_speedY"]);
	return result;
}
