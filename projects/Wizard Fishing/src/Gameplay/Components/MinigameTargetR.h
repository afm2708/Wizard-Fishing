#pragma once
#include "IComponent.h"
#include "Fishmovement.h"
#include "Minigame.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class MinigameTargetR : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MinigameTargetR> Sptr;

	MinigameTargetR();
	virtual ~MinigameTargetR();

	

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(MinigameTargetR);
	virtual nlohmann::json ToJson() const override;
	static MinigameTargetR::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;
	Minigame::Sptr minigame;
	FishMovement::Sptr difficulty;

protected:
	float moveX;
	float moveY;
	float moveSpeedX;
	float moveSpeedY;
	float middleX;
	float middleY;
	float flip;

	GLFWwindow* _window;
};


