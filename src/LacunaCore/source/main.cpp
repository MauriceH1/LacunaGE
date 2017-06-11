#include <LacunaPlatform.h>
#include <LacunaDX12Renderer.h>
// #include <LacunaVRenderer.h>
#include "resources/ResourceManager.h"
#include "resources/SceneResource.h"
#include "system/EntityFactory.h"
#include "system/InputSystem.h"
#include "system/StateSystem.h"

#include "game_objects/Camera.h"
#include "game_objects/components/MeshComponent.h"
#include "game_objects/Components/AudioSourceComponent.h"

#include <moodycamel/concurrentqueue.h>
#include <cstdio>
#include <iostream>
#include <chrono>


lcn::platform::LacunaWindow* lcnCreateWindow();
lcn::graphics::Renderer* lcnCreateRenderer(lcn::platform::LacunaWindow* a_Window);
lcn::resources::ResourceManager* lcnCreateResourceManager(lcn::graphics::Renderer* a_Renderer, char* argv[]);
lcn::resources::GUID lcnLoadAndCreateDefaultPipeline(lcn::resources::ResourceManager* a_ResourceManager);

lcn::resources::SceneResource* lcnCreateScene();
void lcnSetupEntities(lcn::resources::SceneResource* a_Scene);

void UpdateTransforms(moodycamel::ConcurrentQueue<lcn::object::Entity*>* a_EntitiesToUpdate);

void main(int argc, char* argv[])
{
	lcn::platform::LacunaWindow* window = lcnCreateWindow();
	lcn::graphics::Renderer* renderer = lcnCreateRenderer(window);
	lcn::resources::ResourceManager* resourceManager = lcnCreateResourceManager(renderer, argv);
	lcn::resources::GUID pipe = lcnLoadAndCreateDefaultPipeline(resourceManager);

	// ################## SETUP ENTITIES ####################
	lcn::resources::SceneResource* myScene = lcnCreateScene();
	// lcn::object::Entity* cubeParent = lcn::EntityFactory::CreateEntity();
	lcn::object::Entity* cube = resourceManager->LoadModel("assets/cube.obj");
	// lcn::object::Entity* cube2 = resourceManager->LoadModel("assets/cube.obj");
	// lcn::object::Entity* cube3 = resourceManager->LoadModel("assets/cube.obj");
	// cubeParent->AddChild(cube);
	// cubeParent->AddChild(cube2);
	// cubeParent->AddChild(cube3);
	// cubeParent->SetPosition(glm::vec3(5.0f, 1.0f, 2.0f));
	// cube2->SetPosition(glm::vec3(-5.0f, -1.0f, -2.0f));
	// cube3->SetPosition(glm::vec3(-10.f, -2.0f, -4.0f));
	// lcn::object::Entity* sponza = resourceManager->LoadModel("assets/sponza/sponza.obj");

	myScene->AddChild(cube);

	lcnSetupEntities(myScene);

	// ###################### Multi threaded transform update ##########################
	//{
	//	std::queue<lcn::object::Entity*> entitiesToUpdate;
	//	std::thread workers[7];
	//}


	moodycamel::ConcurrentQueue<lcn::object::Entity*> entitiesToUpdate(512, 0, 8);
	entitiesToUpdate.enqueue_bulk(myScene->GetChildren()->begin(), myScene->GetChildren()->size());
	std::thread workers[7];
	for (int i = 0; i < 7; i++)
		workers[i] = std::thread(UpdateTransforms, &entitiesToUpdate);

	UpdateTransforms(&entitiesToUpdate);

	for (int i = 0; i < 7; i++)
		workers[i].join();
	
	// ################## ENTER GAMELOOP #####################
	while(window->HandleMessages() == 0)
	{
		// { // This needs to be cleaner and I need to find a way to multithread without creating new threads every frame.
		// 	entitiesToUpdate.enqueue_bulk(myScene->GetChildren()->begin(), myScene->GetChildren()->size());
		// 	for (int i = 0; i < 7; i++)
		// 		workers[i] = std::thread(UpdateTransforms, &entitiesToUpdate);
		// 	
		// 	UpdateTransforms(&entitiesToUpdate);
		// 	for (int i = 0; i < 7; i++)
		// 		workers[i].join();
		// }

		renderer->Render(myScene);
		lcn::EntityFactory::GetMainCamera()->Update();
	}

	delete resourceManager;
	renderer->Cleanup();

	exit(0);
}

void lcnSetupEntities(lcn::resources::SceneResource* a_Scene)
{
	lcn::object::Entity* camera = lcn::EntityFactory::CreateCamera();
	camera->SetPosition(glm::vec3(0.f, 0.f, 10.0f));

	a_Scene->AddChild(camera);
}

lcn::resources::SceneResource* lcnCreateScene()
{
	lcn::resources::SceneResource* scene = new lcn::resources::SceneResource();
	lcn::StateSystem::SetActiveScene(scene);
	return scene;
}

lcn::platform::LacunaWindow* lcnCreateWindow()
{
	lcn::platform::LacunaWindowOptions a_Options;
	a_Options.KeyUpFunction = &lcn::InputSystem::KeyUp;
	a_Options.KeyDownFunction = &lcn::InputSystem::KeyDown;
	return lcn::platform::CreateWindow(a_Options);
}

lcn::graphics::Renderer* lcnCreateRenderer(lcn::platform::LacunaWindow* a_Window)
{
	lcn::graphics::LacunaDX12Renderer* renderer = new lcn::graphics::LacunaDX12Renderer();
	// lcn::graphics::LacunaVRenderer* renderer = new lcn::graphics::LacunaVRenderer();
	renderer->Initialize(a_Window->GetPlatformHandles());
	return renderer;
}

lcn::resources::ResourceManager* lcnCreateResourceManager(lcn::graphics::Renderer* a_Renderer, char* argv[])
{
	lcn::resources::ResourceManager* resourceManager = new lcn::resources::ResourceManager(argv[0]);
	resourceManager->Initialize(a_Renderer->GetDevice());
	return resourceManager;
}

lcn::resources::GUID lcnLoadAndCreateDefaultPipeline(lcn::resources::ResourceManager* a_ResourceManager)
{
	lcn::resources::GUID VShader = a_ResourceManager->LoadAndCompileShader("shaders/build-in/shaders.hlsl", "VSMain", lcn::resources::EShaderTypes::EShaderTypes_VS);
	lcn::resources::GUID PShader = a_ResourceManager->LoadAndCompileShader("shaders/build-in/shaders.hlsl", "PSMain", lcn::resources::EShaderTypes::EShaderTypes_PS);

	lcn::resources::PipelineParams pipelineParams = {};
	pipelineParams.VertexShader = VShader.ID;
	pipelineParams.PixelShader = PShader.ID;
	return a_ResourceManager->CreatePipeline(pipelineParams);
}

void UpdateTransforms(moodycamel::ConcurrentQueue<lcn::object::Entity*>* a_EntitiesToUpdate)
{
	while (a_EntitiesToUpdate->size_approx() > 0)
	{
		lcn::object::Entity* entity = nullptr;
		a_EntitiesToUpdate->try_dequeue<lcn::object::Entity*>(entity);
		if (entity)
		{
			entity->UpdateTransforms();
			if (entity->GetChildren()->size() > 0) 
				if(!a_EntitiesToUpdate->enqueue_bulk(entity->GetChildren()->begin(), entity->GetChildren()->size())) // LOCKS ON GIGANTIC INSERTS
			{
					std::cout << "MemAlloc Failed!\n" << std::endl;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}
