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

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class MorphMeshRenderer : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<MorphMeshRenderer> Sptr;
	virtual void RenderImGui() override;
	MAKE_TYPENAME(MorphMeshRenderer);
	virtual nlohmann::json ToJson() const override;
	static MorphMeshRenderer::Sptr FromJson(const nlohmann::json& blob);
	enum class Attrib
	{
		POSITION_0 = 0,
		POSITION_1 = 1,
		NORMAL_0 = 2,
		NORMAL_1 = 3,
		UV = 4
	};

	MorphMeshRenderer(/* Gameplay::MeshResource baseMesh, Gameplay::MeshResource targetMesh,
		Gameplay::Material mat */ );
	virtual ~MorphMeshRenderer() = default;
	MorphMeshRenderer(MorphMeshRenderer&&) = default;
	MorphMeshRenderer& operator = (MorphMeshRenderer&&) = default;

	void UpdateData(const Gameplay::MeshResource::Sptr &frame0, const Gameplay::MeshResource::Sptr &frame1, float t);
	float t;
protected:

	//Pointers to the frames we're currently in between.
	Gameplay::MeshResource::Sptr* frame0;
	Gameplay::MeshResource::Sptr* frame1;
	//The t-value for interpolating between our frames.
	VertexArrayObject vao;
	GLFWwindow* _window;
};