#include <LacunaPlatform.h>
#include <LacunaDX12Renderer.h>
// #include <LacunaVRenderer.h>
#include "resources\ResourceManager.h"

#include <cstdio>
#include <iostream>

#include <string>

const char* GetLocation(char* path)
{
	std::string filepath(path);
	size_t last_sep_idx = filepath.find_last_of('\\');
	if(last_sep_idx == std::string::npos)
		last_sep_idx = filepath.find_last_of('/');

	last_sep_idx++;

	char* directory = new char[filepath.substr(0, last_sep_idx).length()];
	strcpy(directory, filepath.substr(0, last_sep_idx).c_str());
	return directory;
}

void main(int argc, char* argv[])
{
	lcn::platform::LacunaWindowOptions a_Options;
	lcn::platform::LacunaWindow* window = lcn::platform::CreateWindow(a_Options);
	
	// Convert to wide strings
	const char* loc = GetLocation(argv[0]);
	const size_t cSize = strlen(loc) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, loc, cSize);

	lcn::graphics::LacunaDX12Renderer* renderer = new lcn::graphics::LacunaDX12Renderer(wc);
	// lcn::graphics::LacunaVRenderer* renderer = new lcn::graphics::LacunaVRenderer();
	renderer->Initialize(window->GetPlatformHandles());

	lcn::resources::ResourceManager* resourceManager = new lcn::resources::ResourceManager();
	resourceManager->Initialize(renderer->GetDevice());
	
	std::string base_path(loc);
	resourceManager->LoadModel((base_path + "Assets/cube.obj").c_str());

	while(window->HandleMessages() == 0)
	{
		renderer->Render();
	}

	renderer->Cleanup();

	exit(0);
}