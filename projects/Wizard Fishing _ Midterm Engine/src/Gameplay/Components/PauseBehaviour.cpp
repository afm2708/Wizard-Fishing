#include "PauseBehaviour.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include <GLFW/glfw3.h>

PauseBehaviour::PauseBehaviour() {
	isPaused = false;
}

void PauseBehaviour::Update(float deltaTime) {
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ESCAPE))
	{
		if (!isPaused)
		{
			isPaused = true;
		}
		else
		{
			isPaused = false;
		}
	}
}

void PauseBehaviour::RenderImGui() {
	//LABEL_LEFT(ImGui::DragFloat, "Speed", &speed);
}

nlohmann::json PauseBehaviour::ToJson() const {
	return {
		//{ "speed", speed }
	};
}

PauseBehaviour::Sptr PauseBehaviour::FromJson(const nlohmann::json& data) {
	PauseBehaviour::Sptr result = std::make_shared<PauseBehaviour>();
	//result->speed = (data["speed"]);
	return result;
}