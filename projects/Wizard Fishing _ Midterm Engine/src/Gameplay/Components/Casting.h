#pragma once
#include "IComponent.h"
#include "Gameplay/Components/TargetComponent.h"

class Casting : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<Casting> Sptr;
	Casting() = default;

	
	virtual void Update(float deltaTime) override;
	float speed;
	bool hasCast;
	TargetComponent::Sptr target;
	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static Casting::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(Casting);
};