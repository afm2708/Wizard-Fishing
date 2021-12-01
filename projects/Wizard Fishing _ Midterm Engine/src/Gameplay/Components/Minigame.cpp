
#include "Gameplay/Components/Minigame.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/FishMovement.h>

Minigame::Minigame() :
	IComponent(),
	moveX(0.0),
	moveY(0.0),
	moveSpeedX(0.05),
	moveSpeedY(0.05),
	middleX(),
	middleY(),
	flip(20.0f),
	minigameActive(false)
{}

Minigame::~Minigame() = default;

void Minigame::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void Minigame::Update(float deltaTime)
{


	if (GetGameObject()->GetScene()->FindObjectByName("Fish")->Get<FishMovement>()->hooked) {
		minigameActive = true;
	}

	if (minigameActive == true) {

		//my head + how far from my head + cos theta
		middleX = cameraCords->GetGameObject()->GetPosition().x - 2.0f * (sin((cameraCords->GetGameObject()->GetRotationEuler().z * 3.141f/ 180.0f)));
		//my head + how far from my head + sin theta
		middleY = cameraCords->GetGameObject()->GetPosition().y + 2.0f * (cos((cameraCords->GetGameObject()->GetRotationEuler().z * 3.141f / 180.0f)));

		flip += 1.0f;

		if (flip >= 40.0f) {
			moveSpeedX = -moveSpeedX;
			moveSpeedY = -moveSpeedY;
			flip = 0.0f;
		}

		moveX += moveSpeedX * (cos((cameraCords->GetGameObject()->GetRotationEuler().z * 3.141f / 180.0f)));
		moveY += moveSpeedY * (sin((cameraCords->GetGameObject()->GetRotationEuler().z * 3.141f / 180.0f)));

		GetGameObject()->SetRotation(glm::vec3(90.0f, 0, cameraCords->GetGameObject()->GetRotationEuler().z));
		GetGameObject()->SetPostion(glm::vec3(middleX + moveX, middleY + moveY, 4.0f));
	}

	else {
		GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -20.0));
		moveX = 0.0f;
	}
	if (glfwGetMouseButton(_window, 0) && flip >= 17.0f && flip <= 23.0f) {
		minigameActive = false;
		GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -20.0));
		moveX = 0.0f;
		moveY =0.0f;
	}
}

void Minigame::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedX, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &moveSpeedY, 0.01f, 0.01f);
}

nlohmann::json Minigame::ToJson() const {
	return {
		{ "move_speedX", (moveSpeedX) },
		{ "move_speedY", (moveSpeedY) },
	};
}

Minigame::Sptr Minigame::FromJson(const nlohmann::json& blob) {
	Minigame::Sptr result = std::make_shared<Minigame>();
	result->moveSpeedX = (blob["move_speedX"]);
	result->moveSpeedY = (blob["move_speedY"]);
	return result;
}
