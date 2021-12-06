#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <sstream>
#include <typeindex>
#include <optional>
#include <string>

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus) 

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

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/WizardMovement.h"
#include "Gameplay/Components/FishMovement.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/Minigame.h"
#include "Gameplay/Components/TargetComponent.h"
#include "Gameplay/Components/Casting.h"
#include "Gameplay/Components/PauseBehaviour.h"
#include "Gameplay/Components/Manabar.h"
#include "Gameplay/Components/ManabarOutline.h"
#include "Gameplay/Components/MinigameTargetL.h"
#include "Gameplay/Components/MinigameTargetR.h"
#include "Gameplay/Components/StaffBehaviour.h"
#include "Gameplay/Components/MorphAnimator.h"
#include "Gameplay/Components/MorphMeshRenderer.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"

//#define LOG_GL_NOTIFICATIONS

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
		case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
			#ifdef LOG_GL_NOTIFICATIONS
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
			#endif
		default: break;
	}
}  

// Stores our GLFW window in a global variable for now
GLFWwindow* window;
// The current size of our window in pixels
glm::ivec2 windowSize = glm::ivec2(800, 800);
// The title of our GLFW window
std::string windowTitle = "Wizard Fishing";

// using namespace should generally be avoided, and if used, make sure it's ONLY in cpp files
using namespace Gameplay;
using namespace Gameplay::Physics;

// The scene that we will be rendering
Scene::Sptr scene = nullptr;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	windowSize = glm::ivec2(width, height);
	if (windowSize.x * windowSize.y > 0) {
		scene->MainCamera->ResizeWindow(width, height);
	}
}

/// <summary>
/// Handles intializing GLFW, should be called before initGLAD, but after Logger::Init()
/// Also handles creating the GLFW window
/// </summary>
/// <returns>True if GLFW was initialized, false if otherwise</returns>
bool initGLFW() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

	//Create a new GLFW window and make it current
	window = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	return true;
}

/// <summary>
/// Handles initializing GLAD and preparing our GLFW window for OpenGL calls
/// </summary>
/// <returns>True if GLAD is loaded, false if there was an error</returns>
bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

/// <summary>
/// Draws a widget for saving or loading our scene
/// </summary>
/// <param name="scene">Reference to scene pointer</param>
/// <param name="path">Reference to path string storage</param>
/// <returns>True if a new scene has been loaded</returns>
bool DrawSaveLoadImGui(Scene::Sptr& scene, std::string& path) {
	// Since we can change the internal capacity of an std::string,
	// we can do cool things like this!
	ImGui::InputText("Path", path.data(), path.capacity());

	// Draw a save button, and save when pressed
	if (ImGui::Button("Save")) {
		scene->Save(path);

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::SaveManifest(newFilename);
	}
	ImGui::SameLine();
	// Load scene from file button
	if (ImGui::Button("Load")) {
		// Since it's a reference to a ptr, this will
		// overwrite the existing scene!
		scene = nullptr;

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::LoadManifest(newFilename);
		scene = Scene::Load(path);

		return true;
	}
	return false;
}

/// <summary>
/// Draws some ImGui controls for the given light
/// </summary>
/// <param name="title">The title for the light's header</param>
/// <param name="light">The light to modify</param>
/// <returns>True if the parameters have changed, false if otherwise</returns>
bool DrawLightImGui(const Scene::Sptr& scene, const char* title, int ix) {
	bool isEdited = false;
	bool result = false;
	Light& light = scene->Lights[ix];
	ImGui::PushID(&light); // We can also use pointers as numbers for unique IDs
	if (ImGui::CollapsingHeader(title)) {
		isEdited |= ImGui::DragFloat3("Pos", &light.Position.x, 0.01f);
		isEdited |= ImGui::ColorEdit3("Col", &light.Color.r);
		isEdited |= ImGui::DragFloat("Range", &light.Range, 0.1f);

		result = ImGui::Button("Delete");
	}
	if (isEdited) {
		scene->SetShaderLight(ix);
	}

	ImGui::PopID();
	return result;
}

int main() {
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Initialize our ImGui helper
	ImGuiHelper::Init(window);

	// Initialize our resource manager
	ResourceManager::Init();

	// Register all our resource types so we can load them from manifest files
	ResourceManager::RegisterType<Texture2D>();
	ResourceManager::RegisterType<TextureCube>();
	ResourceManager::RegisterType<Shader>();
	ResourceManager::RegisterType<Material>();
	ResourceManager::RegisterType<MeshResource>();

	// Register all of our component types so we can load them from files
	ComponentManager::RegisterType<Camera>();
	ComponentManager::RegisterType<RenderComponent>();
	ComponentManager::RegisterType<RigidBody>();
	ComponentManager::RegisterType<TriggerVolume>();
	ComponentManager::RegisterType<RotatingBehaviour>();
	ComponentManager::RegisterType<JumpBehaviour>();
	ComponentManager::RegisterType<MaterialSwapBehaviour>();
	ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
	ComponentManager::RegisterType<SimpleCameraControl>();
	ComponentManager::RegisterType<WizardMovement>();
	ComponentManager::RegisterType<FishMovement>();
	ComponentManager::RegisterType<Minigame>();
	ComponentManager::RegisterType<TargetComponent>();
	ComponentManager::RegisterType<Casting>();
	ComponentManager::RegisterType<PauseBehaviour>();
	ComponentManager::RegisterType<ManaBar>();
	ComponentManager::RegisterType<ManaBarOutline>();
	ComponentManager::RegisterType<MinigameTargetL>();
	ComponentManager::RegisterType<MinigameTargetR>();
	ComponentManager::RegisterType<MorphAnimator>();
	ComponentManager::RegisterType<MorphMeshRenderer>();
	ComponentManager::RegisterType<StaffBehaviour>();

	// GL states, we'll enable depth testing and backface fulling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene) {
		ResourceManager::LoadManifest("manifest.json");
		scene = Scene::Load("scene.json");

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();
	} 
	else {
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials
		Shader::Sptr reflectiveShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" },  
			{ ShaderPartType::Fragment, "shaders/frag_environment_reflective.glsl" }  
		}); 

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr basicShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" },
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		});

		Shader::Sptr morphShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/morph_shader.glsl" },
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		});

		MeshResource::Sptr bobberMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Bobber.obj");
		Texture2D::Sptr	   bobberTex = ResourceManager::CreateAsset<Texture2D>("Textures/BobberTex.png");

		Texture2D::Sptr    boxTex = ResourceManager::CreateAsset<Texture2D>("Textures/BoxTex.png");

		MeshResource::Sptr bridgeMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Bridge.obj");
		Texture2D::Sptr	   bridgeTex = ResourceManager::CreateAsset<Texture2D>("Textures/BridgeTex.png");

		MeshResource::Sptr dockMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Dock.obj");
		Texture2D::Sptr	   dockTex = ResourceManager::CreateAsset<Texture2D>("Textures/DockTex.png");

		MeshResource::Sptr fishMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Fish.obj");
		MeshResource::Sptr fishWiggle1Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/FishWiggle1.obj");
		MeshResource::Sptr fishWiggle2Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/FishWiggle2.obj");
		Texture2D::Sptr	   redfishTex = ResourceManager::CreateAsset<Texture2D>("Textures/RedFishTex.png");
		Texture2D::Sptr	   greenfishTex = ResourceManager::CreateAsset<Texture2D>("Textures/GreenFishTex.png");
		Texture2D::Sptr	   purplefishTex = ResourceManager::CreateAsset<Texture2D>("Textures/PurpleFishTex.png");

		MeshResource::Sptr grassMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Grass.obj");
		Texture2D::Sptr    grassTexture = ResourceManager::CreateAsset<Texture2D>("Textures/GrassTex.png");

		MeshResource::Sptr lakeBottomMesh = ResourceManager::CreateAsset<MeshResource>("Objects/LakeBottom.obj");
		Texture2D::Sptr    lakeBottomTexture = ResourceManager::CreateAsset<Texture2D>("Textures/LakeBottomTex.png");

		MeshResource::Sptr minigamePointerMesh = ResourceManager::CreateAsset<MeshResource>("Objects/MinigamePointer.obj");
		Texture2D::Sptr    minigamePointerTex = ResourceManager::CreateAsset<Texture2D>("Textures/MinigamePointerTex.png");

		MeshResource::Sptr minigameTargetMesh = ResourceManager::CreateAsset<MeshResource>("Objects/MinigameTarget.obj");
		Texture2D::Sptr    minigameTargetTex = ResourceManager::CreateAsset<Texture2D>("Textures/MinigamePointerTargetTex.png");

		MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Monkey.obj");
		Texture2D::Sptr    monkeyTex = ResourceManager::CreateAsset<Texture2D>("Textures/MonkeyTex.png");

		MeshResource::Sptr spellbookMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Spellbook.obj");
		Texture2D::Sptr    spellbookTex = ResourceManager::CreateAsset<Texture2D>("Textures/BookTex.png");

		MeshResource::Sptr staffMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Staff.obj");
		MeshResource::Sptr staffAnim1Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/Staff2.obj");
		MeshResource::Sptr staffAnim2Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/Staff3.obj");
		Texture2D::Sptr    staffTex = ResourceManager::CreateAsset<Texture2D>("Textures/StaffTex.png");

		MeshResource::Sptr tableLeg1Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/TableLeg1.obj");
		MeshResource::Sptr tableLeg2Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/TableLeg2.obj");
		Texture2D::Sptr    tableLegTex = ResourceManager::CreateAsset<Texture2D>("Textures/TableLegTex.png");

		MeshResource::Sptr tableTopMesh = ResourceManager::CreateAsset<MeshResource>("Objects/TableTop.obj");
		Texture2D::Sptr    tableTopTex = ResourceManager::CreateAsset<Texture2D>("Textures/TableTopTex.png");

		MeshResource::Sptr targetMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Target.obj");
		Texture2D::Sptr    targetTex = ResourceManager::CreateAsset<Texture2D>("Textures/TargetTex.png");

		MeshResource::Sptr waterMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Water.obj");
		Texture2D::Sptr    waterTex = ResourceManager::CreateAsset<Texture2D>("Textures/WaterTex.png");

		MeshResource::Sptr wizardMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard.obj");
		//Texture2D::Sptr    wizardTex = ResourceManager::CreateAsset<Texture2D>("Textures/WizardTex.png");

		MeshResource::Sptr titleMesh = ResourceManager::CreateAsset<MeshResource>("Objects/TitleScreen.obj");
		Texture2D::Sptr    titleTex = ResourceManager::CreateAsset<Texture2D>("Textures/WizardFishingTitleTex.png");

		MeshResource::Sptr manaMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Mana.obj");
		Texture2D::Sptr    manaTex = ResourceManager::CreateAsset<Texture2D>("Textures/ManaBarFillTex.png");

		MeshResource::Sptr manaOutlineMesh = ResourceManager::CreateAsset<MeshResource>("Objects/ManaBar.obj");
		Texture2D::Sptr    manaOutlineTex = ResourceManager::CreateAsset<Texture2D>("Textures/ManaBarOutlineTex.png");

		MeshResource::Sptr fireMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Fire.obj");
		Texture2D::Sptr    fireTex = ResourceManager::CreateAsset<Texture2D>("Textures/FireTex.png");

		MeshResource::Sptr forestMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Forrest.obj");
		MeshResource::Sptr forest2Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/Forrest2.obj");
		Texture2D::Sptr    treeTex = ResourceManager::CreateAsset<Texture2D>("Textures/TreeTex.png");
		Texture2D::Sptr    tree2Tex = ResourceManager::CreateAsset<Texture2D>("Textures/Tree2Tex.png");


		MeshResource::Sptr wizardTowerDoorsMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerDoors.obj");
		MeshResource::Sptr wizardTowerPortalMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerPortal.obj");
		MeshResource::Sptr wizardTowerRoofMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerRoof.obj");
		MeshResource::Sptr wizardTowerStoneMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerStone.obj");
		MeshResource::Sptr wizardTowerWindowsMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerWindows.obj");
		MeshResource::Sptr wizardTowerWoodMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerWood.obj");
		MeshResource::Sptr wizardTowerLightStoneMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Wizard_TowerLightStone.obj");
		MeshResource::Sptr boatMesh = ResourceManager::CreateAsset<MeshResource>("Objects/Boat.obj");
		MeshResource::Sptr boat1Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/Boat2.obj");
		MeshResource::Sptr boat2Mesh = ResourceManager::CreateAsset<MeshResource>("Objects/Boat3.obj");

		Texture2D::Sptr    doorTex = ResourceManager::CreateAsset<Texture2D>("Textures/DoorTex.png");
		Texture2D::Sptr    portalTex = ResourceManager::CreateAsset<Texture2D>("Textures/PortalTex.png");
		Texture2D::Sptr    roofTex = ResourceManager::CreateAsset<Texture2D>("Textures/RoofTex.png");
		Texture2D::Sptr    stoneTex = ResourceManager::CreateAsset<Texture2D>("Textures/StoneTex.png");
		Texture2D::Sptr    lightStoneTex = ResourceManager::CreateAsset<Texture2D>("Textures/LightStoneTex.png");
		Texture2D::Sptr    windowTex = ResourceManager::CreateAsset<Texture2D>("Textures/WindowTex.png");




		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		//TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/background/background.jpg");
		Shader::Sptr      skyboxShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/skybox_frag.glsl" }
		});   
		 
		// Create an empty scene
		scene = std::make_shared<Scene>();

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Create our materials
		// This will be our box material, with no environment reflections
		Material::Sptr doorMaterial = ResourceManager::CreateAsset<Material>();
		{
			doorMaterial->Name = "Door";
			doorMaterial->MatShader = basicShader;
			doorMaterial->Texture = doorTex;
			doorMaterial->Shininess = 2.0f;
		}

		Material::Sptr portalMaterial = ResourceManager::CreateAsset<Material>();
		{
			portalMaterial->Name = "Portal";
			portalMaterial->MatShader = basicShader;
			portalMaterial->Texture = portalTex;
			portalMaterial->Shininess = 2.0f;
		}

		Material::Sptr roofMaterial = ResourceManager::CreateAsset<Material>();
		{
			roofMaterial->Name = "Roof";
			roofMaterial->MatShader = basicShader;
			roofMaterial->Texture = roofTex;
			roofMaterial->Shininess = 2.0f;
		}

		Material::Sptr stoneMaterial = ResourceManager::CreateAsset<Material>();
		{
			stoneMaterial->Name = "Stone";
			stoneMaterial->MatShader = basicShader;
			stoneMaterial->Texture = stoneTex;
			stoneMaterial->Shininess = 2.0f;
		}

		Material::Sptr windowMaterial = ResourceManager::CreateAsset<Material>();
		{
			windowMaterial->Name = "Window";
			windowMaterial->MatShader = basicShader;
			windowMaterial->Texture = windowTex;
			windowMaterial->Shininess = 2.0f;
		}

		Material::Sptr lightStoneMaterial = ResourceManager::CreateAsset<Material>();
		{
			lightStoneMaterial->Name = "Light Stone";
			lightStoneMaterial->MatShader = basicShader;
			lightStoneMaterial->Texture = lightStoneTex;
			lightStoneMaterial->Shininess = 2.0f;
		}
		Material::Sptr bobberMaterial = ResourceManager::CreateAsset<Material>();
		{
			bobberMaterial->Name = "Bobber";
			bobberMaterial->MatShader = basicShader;
			bobberMaterial->Texture = bobberTex;
			bobberMaterial->Shininess = 2.0f;
		}

		Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>();
		{
			boxMaterial->Name = "Box";
			boxMaterial->MatShader = basicShader;
			boxMaterial->Texture = boxTex;
			boxMaterial->Shininess = 2.0f;
		}

		Material::Sptr fireMaterial = ResourceManager::CreateAsset<Material>();
		{
			fireMaterial->Name = "Fire";
			fireMaterial->MatShader = basicShader;
			fireMaterial->Texture = fireTex;
			fireMaterial->Shininess = 2.0f;
		}

		Material::Sptr treeMaterial = ResourceManager::CreateAsset<Material>();
		{
			treeMaterial->Name = "Tree";
			treeMaterial->MatShader = basicShader;
			treeMaterial->Texture = treeTex;
			treeMaterial->Shininess = 2.0f;
		}


		Material::Sptr tree2Material = ResourceManager::CreateAsset<Material>();
		{
			tree2Material->Name = "Tree";
			tree2Material->MatShader = basicShader;
			tree2Material->Texture = tree2Tex;
			tree2Material->Shininess = 2.0f;
		}

		Material::Sptr manaMaterial = ResourceManager::CreateAsset<Material>();
		{
			manaMaterial->Name = "Mana bar";
			manaMaterial->MatShader = basicShader;
			manaMaterial->Texture = manaTex;
			manaMaterial->Shininess = 2.0f;
		}

		Material::Sptr manaOutlineMaterial = ResourceManager::CreateAsset<Material>();
		{
			manaOutlineMaterial->Name = "Mana Outline";
			manaOutlineMaterial->MatShader = basicShader;
			manaOutlineMaterial->Texture = manaOutlineTex;
			manaOutlineMaterial->Shininess = 2.0f;
		}

		Material::Sptr bridgeMaterial = ResourceManager::CreateAsset<Material>();
		{
			bridgeMaterial->Name = "Bridge";
			bridgeMaterial->MatShader = basicShader;
			bridgeMaterial->Texture = bridgeTex;
			bridgeMaterial->Shininess = 2.0f;
		}

		Material::Sptr dockMaterial = ResourceManager::CreateAsset<Material>();
		{
			dockMaterial->Name = "Dock";
			dockMaterial->MatShader = basicShader;
			dockMaterial->Texture = dockTex;
			dockMaterial->Shininess = 2.0f;
		}

		Material::Sptr boatMaterial = ResourceManager::CreateAsset<Material>();
		{
			boatMaterial->Name = "Boat";
			boatMaterial->MatShader = morphShader;
			boatMaterial->Texture = dockTex;
			boatMaterial->Shininess = 2.0f;
		}

		Material::Sptr redfishMaterial = ResourceManager::CreateAsset<Material>();
		{
			redfishMaterial->Name = "Red Fish";
			redfishMaterial->MatShader = morphShader;
			redfishMaterial->Texture = redfishTex;
			redfishMaterial->Shininess = 256.0f;
		}

		Material::Sptr greenfishMaterial = ResourceManager::CreateAsset<Material>();
		{
			greenfishMaterial->Name = "Green Fish";
			greenfishMaterial->MatShader = morphShader;
			greenfishMaterial->Texture = greenfishTex;
			greenfishMaterial->Shininess = 256.0f;
		}

		Material::Sptr purplefishMaterial = ResourceManager::CreateAsset<Material>();
		{
			purplefishMaterial->Name = "Purple Fish";
			purplefishMaterial->MatShader = morphShader;
			purplefishMaterial->Texture = purplefishTex;
			purplefishMaterial->Shininess = 256.0f;
		}

		Material::Sptr grassMaterial = ResourceManager::CreateAsset<Material>();
		{
			grassMaterial->Name = "Grass";
			grassMaterial->MatShader = basicShader;
			grassMaterial->Texture = grassTexture;
			grassMaterial->Shininess = 256.0f;
		}

		Material::Sptr lakeBottomMaterial = ResourceManager::CreateAsset<Material>();
		{
			lakeBottomMaterial->Name = "Lake Bottom";
			lakeBottomMaterial->MatShader = basicShader;
			lakeBottomMaterial->Texture = lakeBottomTexture;
			lakeBottomMaterial->Shininess = 256.0f;
		}

		Material::Sptr minigamePointerMaterial = ResourceManager::CreateAsset<Material>();
		{
			minigamePointerMaterial->Name = "Minigame Pointer";
			minigamePointerMaterial->MatShader = basicShader;
			minigamePointerMaterial->Texture = minigamePointerTex;
			minigamePointerMaterial->Shininess = 256.0f;
		}

		Material::Sptr minigameTargetMaterial = ResourceManager::CreateAsset<Material>();
		{
			minigameTargetMaterial->Name = "Minigame Pointer";
			minigameTargetMaterial->MatShader = basicShader;
			minigameTargetMaterial->Texture = minigameTargetTex;
			minigameTargetMaterial->Shininess = 256.0f;
		}

		Material::Sptr titleMaterial = ResourceManager::CreateAsset<Material>();
		{
			titleMaterial->Name = "Title Screen";
			titleMaterial->MatShader = basicShader;
			titleMaterial->Texture = titleTex;
			titleMaterial->Shininess = 256.0f;
		}

		Material::Sptr monkeyMaterial = ResourceManager::CreateAsset<Material>();
		{
			monkeyMaterial->Name = "Monkey";
			monkeyMaterial->MatShader = basicShader;
			monkeyMaterial->Texture = monkeyTex;
			monkeyMaterial->Shininess = 256.0f;
		}

		Material::Sptr staffMaterial = ResourceManager::CreateAsset<Material>();
		{
			staffMaterial->Name = "Staff";
			staffMaterial->MatShader = morphShader;
			staffMaterial->Texture = staffTex;
			staffMaterial->Shininess = 256.0f;
		}


		Material::Sptr spellbookMaterial = ResourceManager::CreateAsset<Material>();
		{
			spellbookMaterial->Name = "Book";
			spellbookMaterial->MatShader = basicShader;
			spellbookMaterial->Texture = spellbookTex;
			spellbookMaterial->Shininess = 256.0f;
		}


		Material::Sptr tableLegMaterial = ResourceManager::CreateAsset<Material>();
		{
			tableLegMaterial->Name = "Table Leg";
			tableLegMaterial->MatShader = basicShader;
			tableLegMaterial->Texture =  tableLegTex;
			tableLegMaterial->Shininess = 256.0f;
		}

		Material::Sptr tableTopMaterial = ResourceManager::CreateAsset<Material>();
		{
			tableTopMaterial->Name = "Table Top";
			tableTopMaterial->MatShader = basicShader;
			tableTopMaterial->Texture = tableTopTex;
			tableTopMaterial->Shininess = 256.0f;
		}

		Material::Sptr targetMaterial = ResourceManager::CreateAsset<Material>();
		{
			targetMaterial->Name = "Target";
			targetMaterial->MatShader = basicShader;
			targetMaterial->Texture = targetTex;
			targetMaterial->Shininess = 256.0f;
		}

		Material::Sptr waterMaterial = ResourceManager::CreateAsset<Material>();
		{
			waterMaterial->Name = "Water";
			waterMaterial->MatShader = basicShader;
			waterMaterial->Texture = waterTex;
			waterMaterial->Shininess = 500.0f;
		}

		// Create some lights for our scene
		scene->Lights.resize(5);
		scene->Lights[0].Position = glm::vec3(5.0f, -5.0f, 50.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 250.0f;

		scene->Lights[1].Position = glm::vec3(5.0f, 25.0f, 50.0f);
		scene->Lights[1].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[1].Range = 250.0f;

		scene->Lights[2].Position = glm::vec3(35.0f, -5.0f, 50.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[2].Range = 250.0f;

		scene->Lights[3].Position = glm::vec3(35.0f, 25.0f, 50.0f);
		scene->Lights[3].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[3].Range = 250.0f;

		scene->Lights[4].Position = glm::vec3(55.0f, 45.0f, 50.0f);
		scene->Lights[4].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[4].Range = 3000.0f;

		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		GameObject::Sptr book = scene->CreateGameObject("Book");
		{
			// Scale up the plane
			book->SetPostion(glm::vec3(59.0, 16.07, 2.4));
			book->SetScale(glm::vec3(0.5F));
			book->SetRotation(glm::vec3(0, 0, -90));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = book->Add<RenderComponent>();
			renderer->SetMesh(spellbookMesh);
			renderer->SetMaterial(spellbookMaterial);

			book->Add<PauseBehaviour>();

		}

		//Wizard model is currently a log
		GameObject::Sptr wizard = scene->CreateGameObject("Wizard");
		{
			// Scale up the plane
			wizard->SetScale(glm::vec3(0.5F));
			wizard->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = wizard->Add<RenderComponent>();
			renderer->SetMesh(wizardMesh);
			renderer->SetMaterial(grassMaterial);

			WizardMovement::Sptr move = wizard->Add<WizardMovement>();
			move->speed = 2;
		}

		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(22.1f, 60.5f, 31.63f));
			camera->SetRotation(glm::vec3(45.0f, 0.0f, 180.0f));

			camera->Add<SimpleCameraControl>();

			Camera::Sptr cam = camera->Add<Camera>();

			camera->Get<SimpleCameraControl>()->player = wizard->Get<WizardMovement>();
			camera->Get<SimpleCameraControl>()->pause = book->Get<PauseBehaviour>();

			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;
		}

		GameObject::Sptr staff = scene->CreateGameObject("Staff");
		{
			// Scale up the plane
			staff->SetScale(glm::vec3(2.0f));
			staff->SetRotation(glm::vec3(0, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = staff->Add<RenderComponent>();
			renderer->SetMesh(staffMesh);
			renderer->SetMaterial(staffMaterial);

			staff->Add<StaffBehaviour>();
			staff->Get<StaffBehaviour>()->cameraCords = camera->Get<SimpleCameraControl>();
			staff->Add<MorphMeshRenderer>();
			staff->Add<MorphAnimator>();
			staff->Get<MorphAnimator>()->SetFrameTime(0.5f);
			std::vector<MeshResource::Sptr> frames;
			frames.push_back(staffMesh);
			frames.push_back(staffAnim1Mesh);
			frames.push_back(staffAnim2Mesh);
			staff->Get<MorphAnimator>()->SetFrames(frames);
		}

		GameObject::Sptr manaOutline = scene->CreateGameObject("Mana Outline");
		{
			// Scale up the plane
			manaOutline->SetScale(glm::vec3(0.05f));
			manaOutline->SetRotation(glm::vec3(0, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = manaOutline->Add<RenderComponent>();
			renderer->SetMesh(manaOutlineMesh);
			renderer->SetMaterial(manaOutlineMaterial);

			manaOutline->Add<ManaBarOutline>();
			manaOutline->Get<ManaBarOutline>()->cameraCords = camera->Get<SimpleCameraControl>();
		}

		GameObject::Sptr mana = scene->CreateGameObject("Mana");
		{
			// Scale up the plane
			mana->SetScale(glm::vec3(0.05f));
			mana->SetRotation(glm::vec3(0, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = mana->Add<RenderComponent>();
			renderer->SetMesh(manaMesh);
			renderer->SetMaterial(manaMaterial);

			mana->Add<ManaBar>();
			mana->Get<ManaBar>()->cameraCords = camera->Get<SimpleCameraControl>();
		}

		GameObject::Sptr MinigamePointer = scene->CreateGameObject("Minigame Pointer");
		{
			// Scale up the plane
			MinigamePointer->SetScale(glm::vec3(0.5F));
			MinigamePointer->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = MinigamePointer->Add<RenderComponent>();
			renderer->SetMesh(minigamePointerMesh);
			renderer->SetMaterial(minigamePointerMaterial);

			MinigamePointer->Add<Minigame>();
			MinigamePointer->Get<Minigame>()->pause = book->Get<PauseBehaviour>();
			MinigamePointer->Get<Minigame>()->cameraCords = camera->Get<SimpleCameraControl>();
		}


		GameObject::Sptr lakeBottom = scene->CreateGameObject("Lake Bottom");
		{

			lakeBottom->SetScale(glm::vec3(0.5F));
			lakeBottom->SetRotation(glm::vec3(90, 0, 0));
			lakeBottom->SetPostion(glm::vec3(0, 0, -0.590));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = lakeBottom->Add<RenderComponent>();
			renderer->SetMesh(lakeBottomMesh);
			renderer->SetMaterial(lakeBottomMaterial);

		}
		// Set up all our sample objects
		GameObject::Sptr grass = scene->CreateGameObject("Grass");
		{
			// Scale up the plane
			grass->SetScale(glm::vec3(0.5F));
			grass->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = grass->Add<RenderComponent>();
			renderer->SetMesh(grassMesh);
			renderer->SetMaterial(grassMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = grass->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(PlaneCollider::Create());
		}

		GameObject::Sptr bridge = scene->CreateGameObject("Bridge");
		{
			// Scale up the plane
			bridge->SetScale(glm::vec3(0.5F));
			bridge->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = bridge->Add<RenderComponent>();
			renderer->SetMesh(bridgeMesh);
			renderer->SetMaterial(bridgeMaterial);

		}

		GameObject::Sptr target = scene->CreateGameObject("Target");
		{
			// Set position in the scene
			target->SetPostion(glm::vec3(39.0f, 16.0f, -1.5f));
			target->SetScale(glm::vec3(1.0F, 0.5F, 1.0f));
			target->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			// Create and attach a render component
			RenderComponent::Sptr renderer = target->Add<RenderComponent>();
			renderer->SetMesh(targetMesh);
			renderer->SetMaterial(targetMaterial);

			target->Add<TargetComponent>();
			target->Get<TargetComponent>()->pause = book->Get<PauseBehaviour>();
		}

		GameObject::Sptr Bobber = scene->CreateGameObject("Bobber");
		{
			Bobber->SetScale(glm::vec3(0.5f));
			Bobber->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			RenderComponent::Sptr renderer = Bobber->Add<RenderComponent>();
			renderer->SetMesh(bobberMesh);
			renderer->SetMaterial(bobberMaterial);

			Bobber->Add<Casting>();

			Bobber->Get<Casting>()->pause = book->Get<PauseBehaviour>();
			Bobber->Get<Casting>()->target = target->Get<TargetComponent>();
		}

		GameObject::Sptr Fish = scene->CreateGameObject("Fish");
		{
			// Set and rotation position in the scene
			Fish->SetPostion(glm::vec3(-1.5f, 0.0f, 1.0f));
			//Fish->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));

			// Add a render component
			RenderComponent::Sptr renderer = Fish->Add<RenderComponent>();
			renderer->SetMesh(fishMesh);
			renderer->SetMaterial(redfishMaterial);
			std::vector<Gameplay::Material::Sptr> materials;
			materials.push_back(redfishMaterial);
			materials.push_back(greenfishMaterial);
			materials.push_back(purplefishMaterial);
			FishMovement::Sptr lerp = Fish->Add<FishMovement>();
			Fish->Get<FishMovement>()->pause = book->Get<PauseBehaviour>();
			lerp->SetSpeed(6);
			lerp->SetMats(materials);
			Fish->Add<MorphMeshRenderer>();
			Fish->Add<MorphAnimator>();
			Fish->Get<MorphAnimator>()->SetFrameTime(0.4f);
			std::vector<MeshResource::Sptr> frames;
			frames.push_back(fishMesh);
			frames.push_back(fishWiggle1Mesh);
			frames.push_back(fishMesh);
			frames.push_back(fishWiggle2Mesh);
			Fish->Get<MorphAnimator>()->SetFrames(frames);
			Fish->Get<MorphAnimator>()->shouldAnimate = true;
		}


		GameObject::Sptr PointerTargetL = scene->CreateGameObject("Pointer Target Left");
		{
			// Scale up the plane
			PointerTargetL->SetScale(glm::vec3(0.5F));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = PointerTargetL->Add<RenderComponent>();
			renderer->SetMesh(minigameTargetMesh);
			renderer->SetMaterial(minigameTargetMaterial);

			PointerTargetL->Add<MinigameTargetL>();
			PointerTargetL->Get<MinigameTargetL>()->pause = book->Get<PauseBehaviour>();
			PointerTargetL->Get<MinigameTargetL>()->minigame = MinigamePointer->Get<Minigame>();

		}

		GameObject::Sptr PointerTargetR = scene->CreateGameObject("Pointer Target Right");
		{
			// Scale up the plane
			PointerTargetR->SetScale(glm::vec3(0.5F));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = PointerTargetR->Add<RenderComponent>();
			renderer->SetMesh(minigameTargetMesh);
			renderer->SetMaterial(minigameTargetMaterial);

			PointerTargetR->Add<MinigameTargetR>();
			PointerTargetR->Get<MinigameTargetR>()->pause = book->Get<PauseBehaviour>();
			PointerTargetR->Get<MinigameTargetR>()->minigame = MinigamePointer->Get<Minigame>();
		}


		GameObject::Sptr water = scene->CreateGameObject("Water");
		{
			// Scale up the plane
			water->SetPostion(glm::vec3(0.0, 0.0, -0.70));
			water->SetScale(glm::vec3(0.5F));
			water->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = water->Add<RenderComponent>();
			renderer->SetMesh(waterMesh);
			renderer->SetMaterial(waterMaterial);

		}

		GameObject::Sptr dock = scene->CreateGameObject("Dock");
		{
			// Scale up the plane
			dock->SetPostion(glm::vec3(0, 0, -1));
			dock->SetScale(glm::vec3(0.5F));
			dock->SetRotation(glm::vec3(90, 0, 0));

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = dock->Add<RenderComponent>();
			renderer->SetMesh(dockMesh);
			renderer->SetMaterial(dockMaterial);

		}

		GameObject::Sptr tabletop = scene->CreateGameObject("Tabletop");
		{
			tabletop->SetScale(glm::vec3(0.5f));
			tabletop->SetRotation(glm::vec3(0, 0, 90));
			tabletop->SetPostion(glm::vec3(0, 0, -1));

			RenderComponent::Sptr renderer = tabletop->Add<RenderComponent>();
			renderer->SetMesh(tableTopMesh);
			renderer->SetMaterial(tableTopMaterial);
		}

		GameObject::Sptr tableleg1 = scene->CreateGameObject("Table Leg 1");
		{
			tableleg1->SetScale(glm::vec3(0.5f));
			tableleg1->SetPostion(glm::vec3(0, 0, -1));
			tableleg1->SetRotation(glm::vec3(0, 0, 90));

			RenderComponent::Sptr renderer = tableleg1->Add<RenderComponent>();
			renderer->SetMesh(tableLeg1Mesh);
			renderer->SetMaterial(tableLegMaterial);
		}

		GameObject::Sptr tableleg2 = scene->CreateGameObject("Table Leg 2");
		{
			tableleg2->SetScale(glm::vec3(0.5f));
			tableleg2->SetPostion(glm::vec3(0, -4.5, -1));
			tableleg2->SetRotation(glm::vec3(0, 0, 90));

			RenderComponent::Sptr renderer = tableleg2->Add<RenderComponent>();
			renderer->SetMesh(tableLeg1Mesh);
			renderer->SetMaterial(tableLegMaterial);
		}

		GameObject::Sptr tableleg3 = scene->CreateGameObject("Table Leg 3");
		{
			tableleg3->SetScale(glm::vec3(0.5f));
			tableleg3->SetPostion(glm::vec3(2, 0, -1));
			tableleg3->SetRotation(glm::vec3(0, 0, 90));

			RenderComponent::Sptr renderer = tableleg3->Add<RenderComponent>();
			renderer->SetMesh(tableLeg1Mesh);
			renderer->SetMaterial(tableLegMaterial);
		}

		GameObject::Sptr tableleg4 = scene->CreateGameObject("Table Leg 4");
		{
			tableleg4->SetScale(glm::vec3(0.5f));
			tableleg4->SetPostion(glm::vec3(0, -4.5, -1));
			tableleg4->SetRotation(glm::vec3(0, 0, 90));

			RenderComponent::Sptr renderer = tableleg4->Add<RenderComponent>();
			renderer->SetMesh(tableLeg2Mesh);
			renderer->SetMaterial(tableLegMaterial);
		}

		GameObject::Sptr wizardTowerDoors = scene->CreateGameObject("Wizard Tower Doors");
		{
			wizardTowerDoors->SetScale(glm::vec3(0.5f));
			wizardTowerDoors->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerDoors->SetRotation(glm::vec3(0, 0, -100));

			RenderComponent::Sptr renderer = wizardTowerDoors->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerDoorsMesh);
			renderer->SetMaterial(doorMaterial);
		}

		GameObject::Sptr wizardTowerPortal = scene->CreateGameObject("Wizard Tower Portal");
		{
			wizardTowerPortal->SetScale(glm::vec3(0.5f));
			wizardTowerPortal->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerPortal->SetRotation(glm::vec3(0, 0, -100));

			RenderComponent::Sptr renderer = wizardTowerPortal->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerPortalMesh);
			renderer->SetMaterial(portalMaterial);
		}

		GameObject::Sptr wizardTowerRoof = scene->CreateGameObject("Wizard Tower Roof");
		{
			wizardTowerRoof->SetScale(glm::vec3(0.5f));
			wizardTowerRoof->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerRoof->SetRotation(glm::vec3(0, 0, -100));

			RenderComponent::Sptr renderer = wizardTowerRoof->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerRoofMesh);
			renderer->SetMaterial(roofMaterial);
		}

		GameObject::Sptr wizardTowerStone = scene->CreateGameObject("Wizard Tower Stone");
		{
			wizardTowerStone->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerStone->SetRotation(glm::vec3(0, 0, -100));
			wizardTowerStone->SetScale(glm::vec3(0.5f));

			RenderComponent::Sptr renderer = wizardTowerStone->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerStoneMesh);
			renderer->SetMaterial(stoneMaterial);
		}

		GameObject::Sptr wizardTowerLightStone = scene->CreateGameObject("Wizard Tower Light Stone");
		{
			wizardTowerLightStone->SetScale(glm::vec3(0.5f));
			wizardTowerLightStone->SetPostion(glm::vec3(1.967f, -2.048f, -1.030f));
			wizardTowerLightStone->SetRotation(glm::vec3(0, 0, -100));


			RenderComponent::Sptr renderer = wizardTowerLightStone->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerLightStoneMesh);
			renderer->SetMaterial(lightStoneMaterial);
		}

		GameObject::Sptr wizardTowerWindow = scene->CreateGameObject("Wizard Tower Widnow");
		{
			wizardTowerWindow->SetScale(glm::vec3(0.5f));
			wizardTowerWindow->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerWindow->SetRotation(glm::vec3(0, 0, -100));

			RenderComponent::Sptr renderer = wizardTowerWindow->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerWindowsMesh);
			renderer->SetMaterial(windowMaterial);
		}

		GameObject::Sptr wizardTowerWood = scene->CreateGameObject("Wizard Tower Wood");
		{
			wizardTowerWood->SetScale(glm::vec3(0.5f));
			wizardTowerWood->SetPostion(glm::vec3(-28.71f, -40.34f, -1.82f));
			wizardTowerWood->SetRotation(glm::vec3(0, 0, -100));

			RenderComponent::Sptr renderer = wizardTowerWood->Add<RenderComponent>();
			renderer->SetMesh(wizardTowerWoodMesh);
			renderer->SetMaterial(dockMaterial);
		}

		GameObject::Sptr Boat = scene->CreateGameObject("Boat");
		{
			Boat->SetScale(glm::vec3(0.5f));
			Boat->SetPostion(glm::vec3(0.0f, 0.0f, -0.81f));
			Boat->SetRotation(glm::vec3(0, 0, -90));


			RenderComponent::Sptr renderer = Boat->Add<RenderComponent>();
			renderer->SetMesh(boatMesh);
			renderer->SetMaterial(boatMaterial);
			Boat->Add<MorphMeshRenderer>();
			Boat->Add<MorphAnimator>();
			Boat->Get<MorphAnimator>()->SetFrameTime(3.0f);
			std::vector<MeshResource::Sptr> frames;
			frames.push_back(boatMesh);
			frames.push_back(boat1Mesh);
			frames.push_back(boatMesh);
			frames.push_back(boat2Mesh);
			Boat->Get<MorphAnimator>()->SetFrames(frames);
			Boat->Get<MorphAnimator>()->shouldAnimate = true;
		}

		GameObject::Sptr fire = scene->CreateGameObject("Fire");
		{
			fire->SetScale(glm::vec3(0.5f));
			fire->SetPostion(glm::vec3(0.0f, 0.0f, -0.6f));
			fire->SetRotation(glm::vec3(0, 0, -90));


			RenderComponent::Sptr renderer = fire->Add<RenderComponent>();
			renderer->SetMesh(fireMesh);
			renderer->SetMaterial(fireMaterial);
		}

		GameObject::Sptr forest1 = scene->CreateGameObject("forest1");
		{
			forest1->SetScale(glm::vec3(0.5f));
			forest1->SetPostion(glm::vec3(0.0f, 0.0f, -0.6f));
			forest1->SetRotation(glm::vec3(0, 0, -90));


			RenderComponent::Sptr renderer = forest1->Add<RenderComponent>();
			renderer->SetMesh(forestMesh);
			renderer->SetMaterial(treeMaterial);
		}

		GameObject::Sptr forest2 = scene->CreateGameObject("forest2");
		{
			forest2->SetScale(glm::vec3(0.5f));
			forest2->SetPostion(glm::vec3(0.0f, 0.0f, -0.6f));
			forest2->SetRotation(glm::vec3(0, 0, -90));


			RenderComponent::Sptr renderer = forest2->Add<RenderComponent>();
			renderer->SetMesh(forest2Mesh);
			renderer->SetMaterial(tree2Material);
		}
		GameObject::Sptr title = scene->CreateGameObject("Title");
		{
			title->SetScale(glm::vec3(0.5f));
			title->SetPostion(glm::vec3(22.0f, 59.0f, 30.0f));
			title->SetRotation(glm::vec3(0.0f, 45.0f, 90.0f));


			RenderComponent::Sptr renderer = title->Add<RenderComponent>();
			renderer->SetMesh(titleMesh);
			renderer->SetMaterial(titleMaterial);
		}
		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
	}


	// We'll use this to allow editing the save/load path
	// via ImGui, note the reserve to allocate extra space
	// for input!
	std::string scenePath = "scene.json"; 
	scenePath.reserve(256); 

	bool isRotating = true;
	float rotateSpeed = 90.0f;

	// Our high-precision timer
	double lastFrame = glfwGetTime();


	BulletDebugMode physicsDebugMode = BulletDebugMode::None;
	float playbackSpeed = 1.0f;

	nlohmann::json editorSceneState;

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGuiHelper::StartFrame();

		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		// Showcasing how to use the imGui library!
		bool isDebugWindowOpen = ImGui::Begin("Debugging");
		if (isDebugWindowOpen) {
			// Draws a button to control whether or not the game is currently playing
			static char buttonLabel[64];
			sprintf_s(buttonLabel, "%s###playmode", scene->IsPlaying ? "Exit Play Mode" : "Enter Play Mode");
			if (ImGui::Button(buttonLabel)) {
				// Save scene so it can be restored when exiting play mode
				if (!scene->IsPlaying) {
					editorSceneState = scene->ToJson();
				}

				// Toggle state
				scene->IsPlaying = !scene->IsPlaying;

				// If we've gone from playing to not playing, restore the state from before we started playing
				if (!scene->IsPlaying) {
					scene = nullptr;
					// We reload to scene from our cached state
					scene = Scene::FromJson(editorSceneState);
					// Don't forget to reset the scene's window and wake all the objects!
					scene->Window = window;
					scene->Awake();
				}
			}

			// Make a new area for the scene saving/loading
			ImGui::Separator();
			if (DrawSaveLoadImGui(scene, scenePath)) {
				// C++ strings keep internal lengths which can get annoying
				// when we edit it's underlying datastore, so recalcualte size
				scenePath.resize(strlen(scenePath.c_str()));

				// We have loaded a new scene, call awake to set
				// up all our components
				scene->Window = window;
				scene->Awake();
			}
			ImGui::Separator();
			// Draw a dropdown to select our physics debug draw mode
			if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDebugMode)) {
				scene->SetPhysicsDebugDrawMode(physicsDebugMode);
			}
			LABEL_LEFT(ImGui::SliderFloat, "Playback Speed:    ", &playbackSpeed, 0.0f, 10.0f);
			ImGui::Separator();
		}

		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update our application level uniforms every frame

		// Draw some ImGui stuff for the lights
		if (isDebugWindowOpen) {
			for (int ix = 0; ix < scene->Lights.size(); ix++) {
				char buff[256];
				sprintf_s(buff, "Light %d##%d", ix, ix);
				// DrawLightImGui will return true if the light was deleted
				if (DrawLightImGui(scene, buff, ix)) {
					// Remove light from scene, restore all lighting data
					scene->Lights.erase(scene->Lights.begin() + ix);
					scene->SetupShaderAndLights();
					// Move back one element so we don't skip anything!
					ix--;
				}
			}
			// As long as we don't have max lights, draw a button
			// to add another one
			if (scene->Lights.size() < scene->MAX_LIGHTS) {
				if (ImGui::Button("Add Light")) {
					scene->Lights.push_back(Light());
					scene->SetupShaderAndLights();
				}
			}
			// Split lights from the objects in ImGui
			ImGui::Separator();
		}

		dt *= playbackSpeed;

		// Perform updates for all components
		scene->Update(dt);

		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);

		// Update our worlds physics!
		scene->DoPhysics(dt);

		// Draw object GUIs
		if (isDebugWindowOpen) {
			scene->DrawAllGameObjectGUIs();
		}
		
		// The current material that is bound for rendering
		Material::Sptr currentMat = nullptr;
		Shader::Sptr shader = nullptr;

		TextureCube::Sptr environment = scene->GetSkyboxTexture();
		if (environment) environment->Bind(0); 

		// Render all our objects
		ComponentManager::Each<RenderComponent>([&](const RenderComponent::Sptr& renderable) {
			// Early bail if mesh not set
			if (renderable->GetMesh() == nullptr) { 
				return;
			}

			// If we don't have a material, try getting the scene's fallback material
			// If none exists, do not draw anything
			if (renderable->GetMaterial() == nullptr) {
				if (scene->DefaultMaterial != nullptr) {
					renderable->SetMaterial(scene->DefaultMaterial);
				} else {
					return;
				}
			}

			// If the material has changed, we need to bind the new shader and set up our material and frame data
			// Note: This is a good reason why we should be sorting the render components in ComponentManager
			if (renderable->GetMaterial() != currentMat) {
				currentMat = renderable->GetMaterial();
				shader = currentMat->MatShader;

				shader->Bind();
				shader->SetUniform("u_CamPos", scene->MainCamera->GetGameObject()->GetPosition());
				currentMat->Apply();
			}

			// Grab the game object so we can do some stuff with it
			GameObject* object = renderable->GetGameObject();

			// Set vertex shader parameters
			shader->SetUniformMatrix("u_ModelViewProjection", viewProj * object->GetTransform());
			shader->SetUniformMatrix("u_Model", object->GetTransform());
			shader->SetUniformMatrix("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(object->GetTransform()))));

			if (object->Has<MorphMeshRenderer>()) {
				shader->SetUniform("t", object->Get<MorphMeshRenderer>()->t);
			}
			// Draw the object
			renderable->GetMesh()->Draw();
		});

		// Use our cubemap to draw our skybox
		scene->DrawSkybox();


		// End our ImGui window
		ImGui::End();

		VertexArrayObject::Unbind();

		lastFrame = thisFrame;
		ImGuiHelper::EndFrame();
		glfwSwapBuffers(window);
	}

	// Clean up the ImGui library
	ImGuiHelper::Cleanup();

	// Clean up the resource manager
	ResourceManager::Cleanup();

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}