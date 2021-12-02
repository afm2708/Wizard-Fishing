#include "Gameplay/Components/TargetComponent.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/Casting.h>

TargetComponent::TargetComponent() :
	IComponent(),
	_mouseSensitivity({ 0.5f, 0.3f }),
	_moveSpeeds(glm::vec3(4.0f)),
	_shiftMultipler(2.0f),
	_currentRot(glm::vec2(0.0f)),
	_isMousePressed(false)
{ }

TargetComponent::~TargetComponent() = default;

void TargetComponent::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void TargetComponent::Update(float deltaTime)
{
	if (!(TargetComponent::pause->isPaused))
	{
		if (glfwGetMouseButton(_window, 0)) {
			if (_isMousePressed == false) {
				glfwGetCursorPos(_window, &_prevMousePos.x, &_prevMousePos.y);
			}
			_isMousePressed = true;
		}
		else {
			_isMousePressed = false;
		}

		glm::dvec2 currentMousePos;
		glfwGetCursorPos(_window, &currentMousePos.x, &currentMousePos.y);

		if (_isMousePressed && !fishing) {


			_currentRot.x += static_cast<float>(currentMousePos.x - _prevMousePos.x) * _mouseSensitivity.x;
			_currentRot.y += static_cast<float>(currentMousePos.y - _prevMousePos.y) * _mouseSensitivity.y;
			glm::quat rotX = glm::angleAxis(glm::radians(_currentRot.x), glm::vec3(0, 0, 1));
			glm::quat rotY = glm::angleAxis(glm::radians(_currentRot.y), glm::vec3(1, 0, 0));
			glm::quat currentRot = rotX * rotY;
			GetGameObject()->SetRotation(currentRot);

			_prevMousePos = currentMousePos;

			glm::vec3 input = glm::vec3(0.0f);
			if (glfwGetKey(_window, GLFW_KEY_UP)) {
				input.z -= _moveSpeeds.x;
			}
			if (glfwGetKey(_window, GLFW_KEY_DOWN)) {
				input.z += _moveSpeeds.x;
			}
			if (glfwGetKey(_window, GLFW_KEY_LEFT)) {
				input.x -= _moveSpeeds.y;
			}
			if (glfwGetKey(_window, GLFW_KEY_RIGHT)) {
				input.x += _moveSpeeds.y;
			}

			if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT)) {
				input *= _shiftMultipler;
			}

			input *= deltaTime;

			glm::vec3 worldMovement = currentRot * glm::vec4(input, 1.0f);
			GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldMovement);
			if (GetGameObject()->GetPosition().z != 0) {
				GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, 0.0f));
			}
			if (glfwGetKey(_window, GLFW_KEY_E)) {
				fishing = true;
				GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -20.0f));
			}
			if (glfwGetKey(_window, GLFW_KEY_Q)) {
				fishing = false;
				GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, 0.0f));
			}

		glm::vec3 worldMovement = currentRot * glm::vec4(input, 1.0f);
		GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldMovement);
		if (GetGameObject()->GetPosition().z != 0 && !fishing) {
			GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, 0.0f));
		}
		if (GetGameObject()->GetScene()->FindObjectByName("Bobber")->Get<Casting>()->hasFinished) {
			fishing = true;
			GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -22.0f));
		}
		if (glfwGetKey(_window, GLFW_KEY_E)) {
			fishing = false;
		}
	}
}

void TargetComponent::RenderImGui()
{

}

nlohmann::json TargetComponent::ToJson() const {
	return {
		{ "move_speed", GlmToJson(_moveSpeeds) },
		{ "shift_mult", _shiftMultipler }
	};
}

TargetComponent::Sptr TargetComponent::FromJson(const nlohmann::json& blob) {
	TargetComponent::Sptr result = std::make_shared<TargetComponent>();
	result->_moveSpeeds       = ParseJsonVec3(blob["move_speed"]);
	result->_shiftMultipler   = JsonGet(blob, "shift_mult", 2.0f);
	return result;
}
