#include <TinyExtender.h>
#include "TinyShaders.h"
#include <TinyWindow.h>

int main()
{
	windowManager::Initialize();
	
	windowManager::AddWindow("Example");
	TinyExtender::InitializeExtensions();
	tinyShaders* shaderManager = new tinyShaders();

	//the shader manager doesn't actually need to be initialized
	shaderManager->LoadShaderProgramsFromConfigFile("Shaders/Shaders.txt");

	glUseProgram(shaderManager->GetShaderProgramByIndex(0)->handle);
	
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glPointSize(20.0f);
	while (!windowManager::GetWindowShouldCloseByName("Example"))
	{
		windowManager::PollForEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, 1);
		windowManager::WindowSwapBuffersByName("Example");
	}
	shaderManager->Shutdown();
	windowManager::ShutDown();
	
	return 0;
}
