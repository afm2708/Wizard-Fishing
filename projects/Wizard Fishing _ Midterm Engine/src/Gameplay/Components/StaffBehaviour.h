#pragma once
#include "IComponent.h"
#include "SimpleCameraControl.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class StaffBehaviour : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<StaffBehaviour> Sptr;

	StaffBehaviour();
	virtual ~StaffBehaviour();
	SimpleCameraControl::Sptr cameraCords;
	

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	float middleX;
	float middleY;
	float rotation;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(StaffBehaviour);
	virtual nlohmann::json ToJson() const override;
	static StaffBehaviour::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;


protected:

	GLFWwindow* _window;
};


