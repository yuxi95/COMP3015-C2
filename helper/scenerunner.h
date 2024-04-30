#include <glad/glad.h>
#include "scene.h"
#include <GLFW/glfw3.h>
#include "glutils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>

class SceneRunner  {


    int fbw, fbh;
	bool debug;           // Set true to enable debug messages

	Scene* runningScene = nullptr;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

    static void mouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    static SceneRunner* singleton;
    GLFWwindow* window;

	float lastX = (float)WIN_WIDTH / 2.0;
	float lastY = (float)WIN_HEIGHT / 2.0;
	bool firstMouse = true;
public:
    SceneRunner(const std::string & windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0) : debug(true) {
        // Initialize GLFW
        if( !glfwInit() ) exit( EXIT_FAILURE );

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        if(debug) 
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        if(samples > 0) {
            glfwWindowHint(GLFW_SAMPLES, samples);
        }

        // Open the window
        window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL, NULL );
        if( ! window ) {
			std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent(window);

        glfwSetCursorPosCallback(window, mouseMoveCallback);

        glfwSetKeyCallback(window, keyCallback);

        glfwSetWindowUserPointer(window, this);

        // Get framebuffer size
        glfwGetFramebufferSize(window, &fbw, &fbh);

        // Load the OpenGL functions.
        if(!gladLoadGL()) { exit(-1); }

        GLUtils::dumpGLInfo();

        // Initialization
        glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
#ifndef __APPLE__
		if (debug) {
			glDebugMessageCallback(GLUtils::debugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		}
#endif

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        singleton = this;
    }

    int run(Scene & scene) {
        scene.setDimensions(fbw, fbh);
        scene.initScene();
        scene.resize(fbw, fbh);

        runningScene = &scene;

        // Enter the main loop
        mainLoop(window, scene);

#ifndef __APPLE__
		if( debug )
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
				GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

		// Close window and terminate GLFW
		glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs(int argc, char ** argv, std::map<std::string, std::string> & sceneData) {
        if( argc < 2 ) {
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        std::string recipeName = argv[1];
        auto it = sceneData.find(recipeName);
        if( it == sceneData.end() ) {
            printf("Unknown recipe: %s\n\n", recipeName.c_str());
            printHelpInfo(argv[0], sceneData);
            exit(EXIT_FAILURE);
        }

        return recipeName;
    }

    Scene* getRunningScene() const { return runningScene; }

private:
    static void printHelpInfo(const char * exeFile,  std::map<std::string, std::string> & sceneData) {
        printf("Usage: %s recipe-name\n\n", exeFile);
        printf("Recipe names: \n");
        for( auto it : sceneData ) {
            printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
        }
    }

    void mainLoop(GLFWwindow * window, Scene & scene) {
        while( ! glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE) ) {
            GLUtils::checkForOpenGLError(__FILE__,__LINE__);
			
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

            scene.update(deltaTime);
            scene.render();
            glfwSwapBuffers(window);

            processInput(window);

            glfwPollEvents();
			int state = glfwGetKey(window, GLFW_KEY_SPACE);
			if (state == GLFW_PRESS)
				scene.animate(!scene.animating());
        }
    }

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_W);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_S);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_A);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_D);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			runningScene->keyPress(GLFW_KEY_Q);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			runningScene->keyPress(GLFW_KEY_E);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_UP);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_DOWN);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_LEFT);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            runningScene->keyPress(GLFW_KEY_RIGHT);

    }
};

inline  void SceneRunner::mouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

    auto sceneRunner = reinterpret_cast<SceneRunner*>(glfwGetWindowUserPointer(window));

	if (sceneRunner->firstMouse)
	{
		sceneRunner->lastX = xpos;
		sceneRunner->lastY = ypos;
		sceneRunner->firstMouse = false;
	}

	float xoffset = xpos - sceneRunner->lastX;
	float yoffset = sceneRunner->lastY - ypos; // reversed since y-coordinates go from bottom to top

	sceneRunner->lastX = xpos;
	sceneRunner->lastY = ypos;
    sceneRunner->getRunningScene()->onMouseMove(xoffset, yoffset);
}

inline void SceneRunner::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto sceneRunner = reinterpret_cast<SceneRunner*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
    {
		sceneRunner->getRunningScene()->keyDown(key);
    }
}
