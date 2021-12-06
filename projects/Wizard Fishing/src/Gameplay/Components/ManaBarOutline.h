#pragma once
#include "IComponent.h"
#include "SimpleCameraControl.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class ManaBarOutline : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<ManaBarOutline> Sptr;

	ManaBarOutline();
	virtual ~ManaBarOutline();
	SimpleCameraControl::Sptr cameraCords;
	

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

	float middleX;
	float middleY;
	float middleZ;
	float rotationX;
	float rotationZ;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(ManaBarOutline);
	virtual nlohmann::json ToJson() const override;
	static ManaBarOutline::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;


protected:

	GLFWwindow* _window;
};


