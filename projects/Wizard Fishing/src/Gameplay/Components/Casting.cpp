#include "Casting.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include <GLFW/glfw3.h>
#include <Gameplay/Components/Minigame.h>
#include <Gameplay/Components/FishMovement.h>
#include <Gameplay/Components/MorphAnimator.h>

Casting::Casting() {
	timer = 0.0f;
	speed = 1;
	hasFinished = false;
	hasCast = false;
	time = 0;

	//points[0] = glm::vec3(0, 4, 4);
	//points[1] = glm::vec3(10, 0, 10);
}

void Casting::SetTarget(glm::vec3 point) {
	points[2] = point;
	points[1] = glm::vec3((points[0].x + points[2].x) / 2, (points[0].y + points[2].y) / 2, 10);
}

void Casting::Update(float deltaTime) {
	glm::vec3 p0, p1, p2;
	if (!(Casting::pause->isPaused))
	{

		points[0] = GetGameObject()->GetScene()->FindObjectByName("Main Camera")->GetPosition();
		if (hasCast) timer += deltaTime;
	}

		if (timer > speed && hasCast)
		{
			timer -= speed;
			//std::cout << "reached";
			hasFinished = true;
			hasCast = false;
		}

		time = timer / speed;

		p0 = points[0];
		p1 = points[1];
		p2 = points[2];




	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_R) == GLFW_PRESS && !hasCast)
	{
		hasCast = true;
		SetTarget(GetGameObject()->GetScene()->FindObjectByName("Target")->GetPosition());
		GetGameObject()->GetScene()->FindObjectByName("Target")->Get<TargetComponent>()->fishing = true;
	}

	if (glfwGetKey(GetGameObject()->GetScene()->Window, GLFW_KEY_E))
	{
		hasFinished = false;
		hasCast = false;
		GetGameObject()->GetScene()->FindObjectByName("Target")->SetPostion(glm::vec3(0, 0, 0));
	}
	
	if (hasCast && !hasFinished)
	{
		GetGameObject()->SetPostion(Bezier(p0, p1, p2, time));
		GetGameObject()->GetScene()->FindObjectByName("Staff")->Get<MorphAnimator>()->shouldAnimate = true;
	}
	else if (!hasCast)
	{
		GetGameObject()->SetPostion(GetGameObject()->GetScene()->FindObjectByName("Main Camera")->GetPosition());
		GetGameObject()->GetScene()->FindObjectByName("Staff")->Get<MorphAnimator>()->shouldAnimate = false;
	}

	if (hasFinished)
	{
		GetGameObject()->SetPostion(p2);
		GetGameObject()->GetScene()->FindObjectByName("Staff")->Get<MorphAnimator>()->shouldAnimate = false;
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