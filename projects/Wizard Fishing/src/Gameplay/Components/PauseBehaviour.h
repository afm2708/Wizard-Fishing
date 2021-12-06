#pragma once
#include "IComponent.h"

class PauseBehaviour : public Gameplay::IComponent{
public:
	typedef std::shared_ptr<PauseBehaviour> Sptr;
	PauseBehaviour();

	virtual void Update(float deltaTime) override;

	bool isPaused;
	int pauseClock = 0;

	virtual void RenderImGui() override;

	virtual nlohmann::json ToJson() const override;
	static PauseBehaviour::Sptr FromJson(const nlohmann::json& data);

	MAKE_TYPENAME(PauseBehaviour);
};