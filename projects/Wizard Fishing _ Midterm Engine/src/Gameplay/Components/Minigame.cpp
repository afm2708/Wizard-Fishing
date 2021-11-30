
#include "Gameplay/Components/Minigame.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

Minigame::Minigame() :
	IComponent(),
	_moveSpeeds(),
	_isSpacePressed(false)
{}

Minigame::~Minigame() = default;

void Minigame::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void Minigame::Update(float deltaTime)
{


	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		_isSpacePressed = true;
	}
	else {
		_isSpacePressed = false;
	}

	if (_isSpacePressed == true) {
		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x + _moveSpeeds * deltaTime, GetGameObject()->GetPosition().y, 0.0f));

		if (GetGameObject()->GetPosition().x >= 20.0f) {
			_moveSpeeds = -abs(_moveSpeeds);
		}
		if (GetGameObject()->GetPosition().x <= -20.0f) {
			_moveSpeeds = abs(_moveSpeeds);
		}
	}

	else {
		GetGameObject()->SetPostion(glm::vec3(0.0, 0.0, -10.0));
	}
}

void Minigame::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat, "Speed       ", &_moveSpeeds, 0.01f, 0.01f);
}

nlohmann::json Minigame::ToJson() const {
	return {
		{ "move_speed", (_moveSpeeds) },
	};
}

Minigame::Sptr Minigame::FromJson(const nlohmann::json& blob) {
	Minigame::Sptr result = std::make_shared<Minigame>();
	result->_moveSpeeds       = (blob["move_speed"]);
	return result;
}
