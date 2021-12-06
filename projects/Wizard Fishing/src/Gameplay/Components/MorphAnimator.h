#pragma once
#include "IComponent.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"

// Graphics
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"

struct GLFWwindow;

class MorphAnimator : public Gameplay::IComponent {
	public:
		typedef std::shared_ptr<MorphAnimator> Sptr;
		virtual void RenderImGui() override;
		MAKE_TYPENAME(MorphAnimator);
		virtual nlohmann::json ToJson() const override;
		static MorphAnimator::Sptr FromJson(const nlohmann::json& blob);

		MorphAnimator();
		~MorphAnimator() = default;

		MorphAnimator(MorphAnimator&&) = default;
		MorphAnimator& operator=(MorphAnimator&&) = default;

		void Update(float deltaTime);

		void SetFrameTime(float);

		void SetFrames(const std::vector<Gameplay::MeshResource::Sptr>);

	protected:

	class AnimData
	{
	public:

	std::vector<Gameplay::MeshResource::Sptr> animFrames;

	//The time inbetween frames.
	float frameTime;
	bool started = false;
	int index0 = 0;

	AnimData();
	~AnimData() = default;
	};

	AnimData data;

	float timer;
	bool forwards;
	
	GLFWwindow* _window;
};