#include "PauseBehaviour.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Minigame.h"
#include "Gameplay/Scene.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include <GLFW/glfw3.h>

PauseBehaviour::PauseBehaviour() {
	isPaused = false;
	pauseClock = 0;
}

void PauseBehaviour::Update(float deltaTime) {
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ESCAPE))
	{
		if (pauseClock == 0) {
			if (!isPaused)
			{
				GetGameObject()->SetPostion(glm::vec3(GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->middleX, GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->middleY, 4.0f));
				GetGameObject()->SetRotation(glm::vec3(90.0f, 0.0f, GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->rotation + 90.0f));
				isPaused = true;
				pauseClock += 1;
			}
			else
			{
				GetGameObject()->SetPostion(glm::vec3(59.0, 16.07, 2.4));
				GetGameObject()->SetRotation(glm::vec3(0, 0, 0));
				isPaused = false;
				pauseClock += 1;
			}
		}
	}
	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_ENTER))
	{
		if (isPaused) {
		glfwSetWindowShouldClose(GetGameObject()->GetScene()->Window, true);
		}
	}
	if (pauseClock >= 1) {
		pauseClock += 1;
	}
	if (pauseClock >= 25) {
		pauseClock = 0;
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