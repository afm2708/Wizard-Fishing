#include "Gameplay/Components/FishMovement.h"

#include "Gameplay/GameObject.h"

#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"


FishMovement::FishMovement() {
	timer = 0.0f;
	index = 0;
	//x bounds 0, 40
	//y bounds -10, 30
	//z bounds 1, -10
	points.push_back(glm::vec3(23, -50, 0));
	for (int i = 0; i < 14; i++) {
	float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (40)));
	float y = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (30 - (-10))));
	float z = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 + 10)));
	points.push_back(glm::vec3(x, y, z));
	}

}

void FishMovement::Update(float deltaTime) {
	if (!(FishMovement::pause->isPaused))
	{
		timer += deltaTime;
		//Ensure we are not "over time" and move to the next segment
		//if necessary.
		while (timer > speed)
		{
			timer -= speed;

			index += 1;

			if (index >= points.size())
				index = 0;
		}

		float time = timer / speed;

		// Neither Catmull nor Bezier make sense with less than 4 points.
		if (points.size() < 4)
		{
			return;
		}

		glm::vec3 p0, p1, p2, p3;
		int p0_index, p1_index, p2_index, p3_index;

		//lerp stuff
		p1_index = index;
		p0_index = (p1_index == 0) ? points.size() - 1 : p1_index - 1;
		p2_index = (p1_index + 1) % points.size();
		p3_index = (p2_index + 1) % points.size();

		p0 = points[p0_index];
		p1 = points[p1_index];
		p2 = points[p2_index];
		p3 = points[p3_index];

		GetGameObject()->SetPostion(Catmull(p0, p1, p2, p3, time));
		GetGameObject()->LookAt(p1);
	}
}

void FishMovement::RenderImGui() {
}

void FishMovement::SetSpeed(int speed) {
	this->speed = speed;
}
void FishMovement::SetPoints(std::vector<glm::vec3> points) {
	this->points = points;
}
nlohmann::json FishMovement::ToJson() const {
	return {
		{ "speed", speed }
	};
}

FishMovement::Sptr FishMovement::FromJson(const nlohmann::json& data) {
	FishMovement::Sptr result = std::make_shared<FishMovement>();
	result->speed = (data["speed"]);
	return result;
}