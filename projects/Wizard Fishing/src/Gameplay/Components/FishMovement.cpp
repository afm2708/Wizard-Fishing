#include "Gameplay/Components/FishMovement.h"

#include "Gameplay/GameObject.h"

#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/Casting.h>
#include <Gameplay/Components/Minigame.h>
#include <Gameplay/Components/RenderComponent.h>

FishMovement::FishMovement() {
	timer = 0.0f;
	index = 0;
    difficulty = 0;
	//x bounds 0, 40
	//y bounds -10, 30
	//z bounds -1, -10
	points.push_back(glm::vec3(23, -50, -4));
    points.push_back(glm::vec3(23, -24, -4));
	lured = false;
	hooked = false;
	for (int i = 0; i < 14; i++) {
	float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (40)));
	float y = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (30 - (-10))));
	float z = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-2 + 10)));
	points.push_back(glm::vec3(x, y, z));
	}
    points.push_back(glm::vec3(23, -24, -2));
}

void FishMovement::Update(float deltaTime) {
	if (!(FishMovement::pause->isPaused))
	{
        if (hooked && !GetGameObject()->GetScene()->FindObjectByName("Minigame Pointer")->Get<Minigame>()->minigameActive) {
            hooked = false;
            lured = false;
            points.pop_back();
            points.pop_back();
            points.push_back(glm::vec3(23, -50, -2));
            points.push_back(glm::vec3(23, -24, -2));
            difficulty = static_cast <int> (rand()) / (static_cast <float> (RAND_MAX / (3)));
            GetGameObject()->GetScene()->FindObjectByName("Fish")->Get<RenderComponent>()->SetMaterial(materials[difficulty]);
            for (int i = 0; i < 14; i++) {
                float x = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (40)));
                float y = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (30 - (-10))));
                float z = -10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-2 + 10)));
                points.push_back(glm::vec3(x, y, z));
            }
            points.push_back(glm::vec3(23, -24, -2));
            index = 0;
            timer = 0;
            GetGameObject()->SetPostion(points[0]);
        }
        if (GetGameObject()->GetScene()->FindObjectByName("Bobber")->Get<Casting>()->hasFinished && !lured
            && abs(GetGameObject()->GetPosition().x - GetGameObject()->GetScene()->FindObjectByName("Bobber")->GetPosition().x) <= 6
            && abs(GetGameObject()->GetPosition().y - GetGameObject()->GetScene()->FindObjectByName("Bobber")->GetPosition().y) <= 6) {
            std::vector<glm::vec3> caughtPoints;
            caughtPoints.push_back(GetGameObject()->GetPosition());
            caughtPoints.push_back(GetGameObject()->GetScene()->FindObjectByName("Bobber")->GetPosition());
            SetPoints(caughtPoints);
            lured = true;
        }
        if (!hooked) timer += deltaTime;
        //Ensure we are not "over time" and move to the next segment
        //if necessary.
        while (timer > speed && !hooked) {
            timer -= speed;

            index += 1;

            if (index >= points.size())
                index = 0;
        }

        float time = timer / speed;

        if (points.size() == 2) {
            if (time > 0.98) {
                hooked = true;
            }
            glm::vec3 lerpCoords((points[1] * time) + points[0] * (1 - time));
            GetGameObject()->SetPostion(lerpCoords);
            GetGameObject()->LookAt(points[1]);
        }

        // Neither Catmull nor Bezier make sense with less than 4 points.
        if (points.size() < 4) {
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
void FishMovement::SetMats(std::vector<Gameplay::Material::Sptr> materials) {
	this->materials = materials;
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