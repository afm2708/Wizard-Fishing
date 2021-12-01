#pragma once
#include "IComponent.h"
#include "SimpleCameraControl.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class Minigame : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<Minigame> Sptr;

	Minigame();
	virtual ~Minigame();
	SimpleCameraControl::Sptr cameraCords;

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(Minigame);
	virtual nlohmann::json ToJson() const override;
	static Minigame::Sptr FromJson(const nlohmann::json& blob);


protected:
	float moveX;
	float moveY;
	float moveSpeedX;
	float moveSpeedY;
	float middleX;
	float middleY;
	float flip;

	bool minigameActive = false;
	GLFWwindow* _window;
};


