#pragma once
#include "IComponent.h"
#include "PauseBehaviour.h"

struct GLFWwindow;

class MorphAnimator : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MorphAnimator> Sptr;

	MorphAnimator();
	virtual ~MorphAnimator();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(MorphAnimator);
	virtual nlohmann::json ToJson() const override;
	static MorphAnimator::Sptr FromJson(const nlohmann::json& blob);

protected:
	
	GLFWwindow* _window;
};