#pragma once
#include "IComponent.h"
#include "SimpleCameraControl.h"
#include "PauseBehaviour.h"

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

	bool minigameActive = false, pressed = false;
	float middleX;
	float middleY;
	float rotation;
	int dif;
	int maxMana, mana;
	glm::vec3(leftEdge) = glm::vec3();
	glm::vec3(rightEdge) = glm::vec3();

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(Minigame);
	virtual nlohmann::json ToJson() const override;
	static Minigame::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;


protected:
	float moveX;
	float moveY;
	float moveSpeedX;
	float moveSpeedY;
	float flip;


	GLFWwindow* _window;
};


