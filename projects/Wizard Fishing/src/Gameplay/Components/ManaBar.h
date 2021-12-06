#pragma once
#include "IComponent.h"
#include "SimpleCameraControl.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class ManaBar : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<ManaBar> Sptr;

	ManaBar();
	virtual ~ManaBar();
	SimpleCameraControl::Sptr cameraCords;
	

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	bool minigameActive = false, pressed;
	int maxMana, mana;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(ManaBar);
	virtual nlohmann::json ToJson() const override;
	static ManaBar::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;


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


