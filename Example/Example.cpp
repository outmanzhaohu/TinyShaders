#include <TinyExtender.h>
using namespace TinyExtender;
#include "TinyShaders.h"
#include <TinyWindow.h>

using namespace TinyWindow;
using namespace TinyShaders;

int main()
{
	windowManager* manager = new windowManager();
	//windowManager::Initialize();
	
	tWindow* window = manager->AddWindow("Example");

	TinyExtender::InitializeExtentions();
	shaderManager* shaders = new shaderManager();

	std::vector<shaderProgram_t*> programs;

	//the shader manager doesn't actually need to be initialized
	shaders->LoadShaderProgramsFromConfigFile("Shaders/Shaders.txt", programs, true);

	glUseProgram(programs[0]->handle);
	
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glPointSize(20.0f);
	while (!window->shouldClose)
	{
		manager->PollForEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, 1);
		window->SwapDrawBuffers();
	}
	shaders->Shutdown();
	manager->ShutDown();
	
	return 0;
}
