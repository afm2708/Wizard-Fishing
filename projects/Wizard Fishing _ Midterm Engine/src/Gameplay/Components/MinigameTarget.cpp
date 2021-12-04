
#include "Gameplay/Components/MinigameTarget.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/Casting.h>

MinigameTarget::MinigameTarget() :
    IComponent(),
    moveX(0.0),
    moveY(0.0),
    moveSpeedX(0.05),
    moveSpeedY(0.05),
    middleX(),
    middleY(),
    flip(20.0f)

{}

MinigameTarget::~MinigameTarget() = default;

void MinigameTarget::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void MinigameTarget::Update(float deltaTime)
{
    if (!(MinigameTarget::pause->isPaused))
    {
        if ((MinigameTarget::minigame->minigameActive))
        {
            GetGameObject()->SetPostion(glm::vec3(MinigameTarget::minigame->middleX, MinigameTarget::minigame->middleY, 3.71f));
            GetGameObject()->SetRotation(glm::vec3(0, 0,MinigameTarget::minigame->rotation -90.0f));
            GetGameObject()->SetScale(glm::vec3(0.30f - 0.1f * MinigameTarget::difficulty->difficulty, 0, 0.1f));
            std::cout << MinigameTarget::difficulty->difficulty << std::endl;
        }
        else {
            GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -20.0));

        }
 
    }

}

void MinigameTarget::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedX, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedY, 0.01f, 0.01f);
}

nlohmann::json MinigameTarget::ToJson() const {
	return {
		{ "move_speedX", (moveSpeedX) },
		{ "move_speedY", (moveSpeedY) },
	};
}

MinigameTarget::Sptr MinigameTarget::FromJson(const nlohmann::json& blob) {
    MinigameTarget::Sptr result = std::make_shared<MinigameTarget>();
	result->moveSpeedX = (blob["move_speedX"]);
	result->moveSpeedY = (blob["move_speedY"]);
	return result;
}
