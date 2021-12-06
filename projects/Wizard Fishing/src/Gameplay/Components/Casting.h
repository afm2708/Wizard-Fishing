#pragma once
#include "IComponent.h"
#include "Gameplay/Components/TargetComponent.h"
#include "PauseBehaviour.h"

class Casting : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<Casting> Sptr;
	Casting();

	void SetTarget(glm::vec3 point);

	virtual void Update(float deltaTime) override;
	glm::vec3 points [3];
	float speed, timer;
	float time;
	bool hasCast;
	bool hasFinished;

	glm::vec3 Bezier(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t)
	{
		return (1-t) * (1 - t) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
	}

	PauseBehaviour::Sptr pause;
	TargetComponent::Sptr target;
	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static Casting::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(Casting);
};