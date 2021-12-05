#include "Gameplay/Components/TargetComponent.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/Casting.h>
#include <Gameplay/Components/Minigame.h>
#include <Gameplay/Components/FishMovement.h>

TargetComponent::TargetComponent() :
	IComponent(),
	_mouseSensitivity({ 0.5f, 0.3f }),
	_moveSpeeds(glm::vec3(4.0f)),
	_shiftMultipler(2.0f),
	_currentRot(glm::vec2(0.0f))
{ }

TargetComponent::~TargetComponent() = default;

void TargetComponent::Awake() {
	_window = GetGameObject()->GetScene()->Window;
}

void TargetComponent::Update(float deltaTime)
{
	if (!(TargetComponent::pause->isPaused))
	{
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
            glm::quat currentRot = GetGameObject()->GetScene()->FindObjectByName("Main Camera")->GetRotation();

            glm::vec3 worldMovement = currentRot * glm::vec4(input, 1.0f);
            //x bounds 0, 40
            //y bounds -10, 30
            GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldMovement);
            if (GetGameObject()->GetPosition().x < 0) GetGameObject()->SetPostion(glm::vec3(0.0f, GetGameObject()->GetPosition().y, -0.7f));
            if (GetGameObject()->GetPosition().x > 40) GetGameObject()->SetPostion(glm::vec3(40.0f, GetGameObject()->GetPosition().y, -0.7f));
            if (GetGameObject()->GetPosition().y < -10) GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, -10.0f, -0.7f));
            if (GetGameObject()->GetPosition().y > 30) GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, 30.0f, -0.7f));
        if (GetGameObject()->GetScene()->FindObjectByName("Bobber")->Get<Casting>()->hasFinished && fishing) {
            fishing = true;
            GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -22.0f));
        }
        else if (GetGameObject()->GetPosition().z != -0.7f) {
            GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y, -0.7f));
        }
        if (!GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->minigameActive &&
            GetGameObject()->GetScene()->FindObjectByName("Fish")->Get<FishMovement>()->hooked) {
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
