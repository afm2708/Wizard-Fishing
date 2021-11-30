#pragma once
#include "IComponent.h"

/// <summary>
/// movement component for fish
/// </summary>
class FishMovement : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<FishMovement> Sptr;

	FishMovement();
	
	std::vector<glm::vec3> points;

	float timer, speed;

	int index;

	virtual void Update(float deltaTime) override;

	virtual void RenderImGui() override;

	glm::vec3 Catmull(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t)
	{
		return 0.5f * (2.0f * p1 + t * (-p0 + p2)
			+ t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3)
			+ t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3));
	}

	void SetSpeed(int);

	void SetPoints(std::vector<glm::vec3>);

	virtual nlohmann::json ToJson() const override;
	static std::shared_ptr<FishMovement> FromJson(const nlohmann::json&);

	MAKE_TYPENAME(FishMovement)
};

