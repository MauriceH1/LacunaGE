#include <LacunaPlatform.h>
#include <LacunaDX12Renderer.h>
// #include <LacunaVRenderer.h>
#include "resources/ResourceManager.h"
#include "system/EntityFactory.h"
#include "system/InputSystem.h"

#include "game_objects/Camera.h"
#include "game_objects/components/MeshComponent.h"
#include "game_objects/Components/AudioSourceComponent.h"

#include <cstdio>
#include <iostream>

void main(int argc, char* argv[])
{
	lcn::platform::LacunaWindowOptions a_Options;
	a_Options.KeyUpFunction = &lcn::InputSystem::KeyUp;
	a_Options.KeyDownFunction = &lcn::InputSystem::KeyDown;
	lcn::platform::LacunaWindow* window = lcn::platform::CreateWindow(a_Options);

	lcn::graphics::LacunaDX12Renderer* renderer = new lcn::graphics::LacunaDX12Renderer();
	// lcn::graphics::LacunaVRenderer* renderer = new lcn::graphics::LacunaVRenderer();
	renderer->Initialize(window->GetPlatformHandles());

	lcn::resources::ResourceManager* resourceManager = new lcn::resources::ResourceManager(argv[0]);
	resourceManager->Initialize(renderer->GetDevice());

	lcn::resources::GUID VShader = resourceManager->LoadAndCompileShader("shaders/build-in/shaders.hlsl", "VSMain", lcn::resources::EShaderTypes::EShaderTypes_VS);
	lcn::resources::GUID PShader = resourceManager->LoadAndCompileShader("shaders/build-in/shaders.hlsl", "PSMain", lcn::resources::EShaderTypes::EShaderTypes_PS);

	lcn::resources::PipelineParams pipelineParams = {};
	pipelineParams.VertexShader = VShader.ID;
	pipelineParams.PixelShader = PShader.ID;
	lcn::resources::GUID pipe = resourceManager->CreatePipeline(pipelineParams);

	resourceManager->LoadModel("assets/cube.obj");

	// ################## SETUP ENTITIES ####################
	auto sceneRoot = lcn::EntityFactory::CreateEntity();
	sceneRoot->SetPosition(glm::vec3(2.0f, 5.0f, 0.0f));

	auto entity = lcn::EntityFactory::CreateEntity();
	entity->AddComponent(new lcn::object::MeshComponent());
	entity->SetPosition(glm::vec3(2.0f, 5.0f, -10.0f));
	sceneRoot->AddChild(entity);

	auto camera = lcn::EntityFactory::CreateCamera();
	camera->SetPosition(glm::vec3(0.f, 0.f, 10.0f));
	sceneRoot->AddChild((lcn::object::Entity*)entity);


	// ################## ENTER GAMELOOP #####################
	while(window->HandleMessages() == 0)
	{
		renderer->Render(sceneRoot);
		camera->Update();
	}

	delete resourceManager;
	renderer->Cleanup();

	exit(0);
}