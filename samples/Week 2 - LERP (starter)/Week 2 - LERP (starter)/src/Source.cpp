/*
Week 2 tutorial sample - LERP
Quinn Daggett 2021
*/

#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "Logging.h"

#include <memory>

using namespace nou;

// TODO: create a templated LERP function
template<class T>
T Interpolate(T a, T b, float time) {
	T c = a * (1 - time) + b * time;
	return c;
}

int main()
{
	// Create window and set clear color
	App::Init("Week 2 tutorial - LERP", 800, 600);
	App::SetClearColor(glm::vec4(0.0f, 0.27f, 0.4f, 1.0f));

	App::Tick();

	// Load vertex and fragment shaders
	std::unique_ptr vs_litShader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_litShader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	// Activate shader program
	ShaderProgram shaderProgram = ShaderProgram({ vs_litShader.get(), fs_litShader.get() });

	// Create and load mesh for duck
	Mesh mesh_duck;
	GLTF::LoadMesh("duck/Duck.gltf", mesh_duck);

	// Create material and load textures for duck
	Texture2D tex2D_duck = Texture2D("duck/DuckCM.png");
	Material mat_duck(shaderProgram);
	mat_duck.AddTexture("albedo", tex2D_duck);

	// Create and set up camera
	Entity ent_camera = Entity::Create();
	CCamera& cam = ent_camera.Add<CCamera>(ent_camera);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	ent_camera.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	// Creating duck entity
	Entity ent_duck = Entity::Create();
	ent_duck.Add<CMeshRenderer>(ent_duck, mesh_duck, mat_duck);
	ent_duck.transform.m_scale = glm::vec3(0.005f, 0.005f, 0.005f);
	ent_duck.transform.m_pos = glm::vec3(0.0f, -1.0f, 0.0f);
	ent_duck.transform.m_rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// TODO: define necessary variables for LERPing
	float time = 0.0f;

	// Main loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		App::FrameStart();
		float dTime = App::GetDeltaTime();

		// TODO: LERP duck properties
		ent_duck.transform.m_scale = Interpolate(ent_duck.transform.m_scale, glm::vec3(0.01f, 0.01f, 0.01f), time);
		ent_duck.transform.m_pos = Interpolate(ent_duck.transform.m_pos, glm::vec3(0.0f, 0.0f, 0.7f), time);
		ent_duck.transform.m_rotation = Interpolate(ent_duck.transform.m_rotation, glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), time);
		mat_duck.m_color = Interpolate(mat_duck.m_color, glm::vec3(0.7f, 0.3f, 1.0f), time);

		if (time < 1.0f) {
			time += dTime/100;
		}

		// Update camera and duck, draw duck
		ent_camera.Get<CCamera>().Update();

		ent_duck.transform.RecomputeGlobal();

		ent_duck.Get<CMeshRenderer>().Draw();

		// Draw everything we just did to the screen
		App::SwapBuffers();
	}

	// Destroy window
	App::Cleanup();

	return 0;
}