#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"
#include <iostream>

#include "imgui.h"

#include <memory>

using namespace nou;

std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Mesh> mesh_ducky, mesh_box;
std::unique_ptr<Texture2D> tex2D_ducky;
std::unique_ptr<Material> mat_ducky, mat_unselected, mat_selected, mat_line;;

// This helps keep our main() clean
void LoadDefaultResources();

int main() {
	// Initialization
	App::Init("Wizard Fishing", 800, 600);
	App::SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Reminder: SetClearColor uses RGB values of 0-1 instead of 0-255
	// Initialize ImGui
	App::InitImgui();

	// Load in our model/texture resources
	LoadDefaultResources();

	// horizontal angle : toward -Z
	float horizontalAngle = 180.0f;
	float absoluteAngle = 180.0f;
	//mouse sensitivity
	float sensitivity = 2.0f;

	// Create and set up camera
	Entity ent_camera = Entity::Create();
	CCamera& cam = ent_camera.Add<CCamera>(ent_camera);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	ent_camera.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	// Creating duck entity
	Entity ent_ducky = Entity::Create();
	ent_ducky.Add<CMeshRenderer>(ent_ducky, *mesh_ducky, *mat_ducky);
	ent_ducky.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	ent_ducky.transform.m_pos = glm::vec3(0.0f, -1.5f, 2.0f);
	ent_ducky.transform.m_rotation = glm::angleAxis(glm::radians(horizontalAngle - 90), glm::vec3(0.0f, 1.0f, 0.0f));

	// Set up waypoint container
	std::vector<std::unique_ptr<Entity>> points;

	App::Tick();

	// Update loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		App::FrameStart();

		// Update our LERP timers
		float deltaTime = App::GetDeltaTime();

		// Get mouse position
		double xpos, ypos;
		glfwGetCursorPos(App::GetWindow(), &xpos, &ypos);
		// Reset mouse position for next frame
		int width, height;
		glfwGetWindowSize(App::GetWindow(), &width, &height);
		glfwSetCursorPos(App::GetWindow(), width / 2, height / 2);
		horizontalAngle = sensitivity * deltaTime * float(width / 2 - xpos);
		absoluteAngle += sensitivity * deltaTime * float(width / 2 - xpos);
		ent_camera.transform.m_pos = ent_ducky.transform.m_pos;
		ent_ducky.transform.m_rotation = glm::rotate(ent_ducky.transform.m_rotation, glm::radians(horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		ent_camera.transform.m_rotation = glm::rotate(ent_camera.transform.m_rotation, glm::radians(horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		if (Input::GetKey(GLFW_KEY_W)) {
			//ent_ducky.transform.m_pos.z -= 2.0f * deltaTime;
			ent_ducky.transform.m_pos.x += 2.0f * deltaTime * (float)sin(glm::radians(absoluteAngle));
			ent_ducky.transform.m_pos.z += 2.0f * deltaTime * (float)cos(glm::radians(absoluteAngle));
		}

		if (Input::GetKey(GLFW_KEY_A)) {
			//ent_ducky.transform.m_pos.x -= 2.0f * deltaTime;
			ent_ducky.transform.m_pos.x -= 2.0f * deltaTime * (float)sin(glm::radians(absoluteAngle - 90));
			ent_ducky.transform.m_pos.z -= 2.0f * deltaTime * (float)cos(glm::radians(absoluteAngle - 90));
		}

		if (Input::GetKey(GLFW_KEY_S)) {
			//ent_ducky.transform.m_pos.z += 2.0f * deltaTime;
			ent_ducky.transform.m_pos.x -= 2.0f * deltaTime * (float)sin(glm::radians(absoluteAngle));
			ent_ducky.transform.m_pos.z -= 2.0f * deltaTime * (float)cos(glm::radians(absoluteAngle));
		}

		if (Input::GetKey(GLFW_KEY_D)) {
			//ent_ducky.transform.m_pos.x += 2.0f * deltaTime;
			ent_ducky.transform.m_pos.x -= 2.0f * deltaTime * (float)sin(glm::radians(absoluteAngle + 90));
			ent_ducky.transform.m_pos.z -= 2.0f * deltaTime * (float)cos(glm::radians(absoluteAngle + 90));
		}

		if (Input::GetKeyDown(GLFW_KEY_Q)) {
			points.push_back(Entity::Allocate());
			points.back()->Add<CMeshRenderer>(*points.back(), *mesh_box, *mat_unselected);
			points.back()->transform.m_scale = glm::vec3(0.1f, 0.1f, 0.1f);
			points.back()->transform.m_pos = ent_ducky.transform.m_pos;
		}

		ent_camera.transform.m_pos = ent_ducky.transform.m_pos + glm::vec3(0.0f, 1.5f, 0.0f);

		// Update camera
		ent_camera.Get<CCamera>().Update();

		// Update duck transform
		ent_ducky.transform.RecomputeGlobal();

		// Update transforms on all our points
		for (int i = 0; i < points.size(); i++)
		{
			points[i]->transform.RecomputeGlobal();
		}

		// Draw our points
		for (int i = 0; i < points.size(); i++)
		{
			points[i]->Get<CMeshRenderer>().Draw();
		}

		// Draw duck
		ent_ducky.Get<CMeshRenderer>().Draw();

		App::SwapBuffers();
	}

	App::Cleanup();

	return 0;
}
void LoadDefaultResources()
{
	// Load in the shaders we will need and activate them
	// Textured lit shader
	std::unique_ptr vs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_texLitShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> texLit = { vs_texLitShader.get(), fs_texLitShader.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	// Untextured lit shader
	std::unique_ptr vs_litShader = std::make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_litShader = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> lit = { vs_litShader.get(), fs_litShader.get() };
	prog_lit = std::make_unique<ShaderProgram>(lit);

	// Untextured unlit shader
	std::unique_ptr vs_unlitShader = std::make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_unlitShader = std::make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);
	std::vector<Shader*> unlit = { vs_unlitShader.get(), vs_unlitShader.get() };
	prog_unlit = std::make_unique<ShaderProgram>(unlit);

	// Set up box
	mesh_box = std::make_unique<Mesh>();
	GLTF::LoadMesh("box/Box.gltf", *mesh_box);

	// Set up duck
	mesh_ducky = std::make_unique<Mesh>();
	GLTF::LoadMesh("duck/Duck.gltf", *mesh_ducky);
	tex2D_ducky = std::make_unique<Texture2D>("duck/DuckCM.png");

	// Set up duck material
	mat_ducky = std::make_unique<Material>(*prog_texLit);
	mat_ducky->AddTexture("albedo", *tex2D_ducky);

	// Set up point and line materials
	mat_unselected = std::make_unique<Material>(*prog_lit);
	mat_unselected->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	mat_selected = std::make_unique<Material>(*prog_lit);
	mat_selected->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	mat_line = std::make_unique<Material>(*prog_unlit);
	mat_line->m_color = glm::vec3(1.0f, 1.0f, 1.0f);
}
