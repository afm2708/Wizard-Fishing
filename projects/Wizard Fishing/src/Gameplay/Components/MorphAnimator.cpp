#include "Gameplay/Components/MorphAnimator.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/MorphMeshRenderer.h>

MorphAnimator::AnimData::AnimData()
{
	index0 = 0;
}

MorphAnimator::MorphAnimator()
{
	data = AnimData();
	timer = 0.0f;
	forwards = true;
}

void MorphAnimator::SetFrameTime(float f) {
	data.frameTime = f;
}

void MorphAnimator::SetFrames(const std::vector<Gameplay::MeshResource::Sptr> loadedFrames) {
	if (data.animFrames.size() == NULL) {
		data.animFrames.clear();
	}

	for (int i = 0; i < loadedFrames.size(); i++) {
		data.animFrames.push_back(loadedFrames[i]);
	}

	data.index0 = 0;
}

void MorphAnimator::Update(float deltaTime)
{
	float t;
	int index1;

	timer += deltaTime;

	if (timer > data.frameTime) {
		timer = 0.f;

		data.index0 = (data.index0 < (data.animFrames.size() - 1)) ? data.index0 + 1 : 0;
	}

	t = timer / data.frameTime;
	index1 = ((data.index0 + 1) >= data.animFrames.size()) ? 0 : (data.index0 + 1);

	GetGameObject()->Get<MorphMeshRenderer>()->UpdateData(data.animFrames[data.index0], data.animFrames[index1], t);
}

void MorphAnimator::RenderImGui()
{

}

nlohmann::json MorphAnimator::ToJson() const {
	return {
	};
}

MorphAnimator::Sptr MorphAnimator::FromJson(const nlohmann::json& blob) {
	MorphAnimator::Sptr result = std::make_shared<MorphAnimator>();
	return result;
}
