#include "Casting.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include <GLFW/glfw3.h>

void Casting::Update(float deltaTime) {
	if (glfwGetMouseButton(GetGameObject()->GetScene()->Window, 0))
	{
		hasCast = true;
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E))
	{
		hasCast = false;
	}
	
	if (hasCast = true)
	{
		GetGameObject()->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	else
	{
		GetGameObject()->SetPostion(GetGameObject()->GetScene()->FindObjectByName("Main Camera")->GetPosition());
	}
}

void Casting::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Speed", &speed);
}

nlohmann::json Casting::ToJson() const {
	return {
		{ "speed", speed }
	};
}

Casting::Sptr Casting::FromJson(const nlohmann::json& data) {
	Casting::Sptr result = std::make_shared<Casting>();
	result->speed = (data["speed"]);
	return result;
}