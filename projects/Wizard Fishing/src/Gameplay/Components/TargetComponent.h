#pragma once
#include "IComponent.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class TargetComponent : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<TargetComponent> Sptr;

	TargetComponent();
	virtual ~TargetComponent();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(TargetComponent);
	virtual nlohmann::json ToJson() const override;
	static TargetComponent::Sptr FromJson(const nlohmann::json& blob);
	PauseBehaviour::Sptr pause;
	bool fishing = false;

protected:
	float _shiftMultipler;

	glm::vec2 _mouseSensitivity;
	glm::vec3 _moveSpeeds;
	glm::dvec2 _prevMousePos;
	glm::vec2 _currentRot;

	GLFWwindow* _window;
};