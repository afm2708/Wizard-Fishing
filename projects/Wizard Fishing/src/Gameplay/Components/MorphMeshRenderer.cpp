#include "Gameplay/Components/MorphMeshRenderer.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include <Gameplay/Components/RenderComponent.h>

MorphMeshRenderer::MorphMeshRenderer(/*Gameplay::MeshResource baseMesh, Gameplay::MeshResource targetMesh, Gameplay::Material mat*/)
{

}

void MorphMeshRenderer::UpdateData(const Gameplay::MeshResource::Sptr &frame0, const Gameplay::MeshResource::Sptr &frame1, float t)
{
	VertexArrayObject::Sptr vao = frame0->Mesh;
	const VertexBuffer::Sptr vboPos = frame1->Mesh->GetBufferBinding(AttribUsage::Position)->Buffer;
	const VertexBuffer::Sptr vboNorm = frame1->Mesh->GetBufferBinding(AttribUsage::Normal)->Buffer;
	const VertexArrayObject::VertexDeclaration dec = frame1->Mesh->GetVDecl();
	std::vector<BufferAttribute> positionBuffer{BufferAttribute(4, 3, AttributeType::Float, dec.at(0).Stride, dec.at(0).Offset, AttribUsage::Position) };
	std::vector<BufferAttribute> normalBuffer{BufferAttribute(5, 3, AttributeType::Float, dec.at(2).Stride, dec.at(2).Offset, AttribUsage::Normal) };
	vao->AddVertexBuffer(vboPos, positionBuffer);
	vao->AddVertexBuffer(vboNorm, normalBuffer);

	GetGameObject()->Get<RenderComponent>()->SetVao(vao);

	this->t = t;
}

void MorphMeshRenderer::RenderImGui()
{

}

nlohmann::json MorphMeshRenderer::ToJson() const {
	return {
	};
}

MorphMeshRenderer::Sptr MorphMeshRenderer::FromJson(const nlohmann::json& blob) {
	MorphMeshRenderer::Sptr result = std::make_shared<MorphMeshRenderer>();
	return result;
}
