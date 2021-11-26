#include "Gameplay/Components/WizardMovement.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include <GLFW/glfw3.h>

void WizardMovement::Update(float deltaTime) {
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_W) == GLFW_PRESS) {

		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y + speed * deltaTime, 0.0f));
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_S) == GLFW_PRESS) {
	
		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y - speed * deltaTime, 0.0f));
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_D) == GLFW_PRESS) {

		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x + speed * deltaTime, GetGameObject()->GetPosition().y, 0.0f));

	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_A) == GLFW_PRESS) {

		GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetPosition().x - speed * deltaTime, GetGameObject()->GetPosition().y, 0.0f));
		
	}

}

void WizardMovement::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Speed", &speed);
}

nlohmann::json WizardMovement::ToJson() const {
	return {
		{ "speed", speed }
	};
}

WizardMovement::Sptr WizardMovement::FromJson(const nlohmann::json& data) {
	WizardMovement::Sptr result = std::make_shared<WizardMovement>();
	result->speed = (data["speed"]);
	return result;
}
