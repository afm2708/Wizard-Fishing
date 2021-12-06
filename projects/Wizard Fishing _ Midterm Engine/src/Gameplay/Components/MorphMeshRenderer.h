#pragma once
#include "IComponent.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class MorphMeshRenderer : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MorphMeshRenderer> Sptr;

	MorphMeshRenderer();
	virtual ~MorphMeshRenderer();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(MorphMeshRenderer);
	virtual nlohmann::json ToJson() const override;
	static MorphMeshRenderer::Sptr FromJson(const nlohmann::json& blob);

protected:

	GLFWwindow* _window;
};