#include "helper/scene.h"
#include "helper/scenerunner.h"
#include "scenebasic_uniform.h"
#include "sceneobjects_uniform.h"


int main(int argc, char* argv[])
{
	SceneRunner runner("Shader_Basics");
	
	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new sceneobjects_uniform( ));

	return runner.run(*scene);
}