#ifndef SCENEOBJECTS_UNIFORM_H
#define SCENEOBJECTS_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <learnopengl/model.h>
#include <learnopengl/camera.h>

class sceneobjects_uniform : public Scene
{
private:
    glm::vec3       light_dir;
	GLuint          depth_FBO = 0;
	GLuint          depth_texture = 0;
	GLuint          proj_texture = 0;

	// GLuint          color_texture = 0;
    Shader          depth_prog;
    glm::mat4       shadowmap_viewproj;
    Shader          shadowmap_prog;
    Shader          screen_prog;

    glm::vec3       player_mov = {0.f, 0.f, 0.f};
    glm::vec3       player_pos = { 15.0f, -1.0f, 15.0f };
    glm::mat4       player_rm = glm::mat4(1.f);

    glm::vec3       treasure_pos;
    float           treasure_timer = 0.f;
    int             score = 0;
    
    GLuint vaoHandle;

    float angle;

    std::vector<Model*> scene_objects;;

    Model house;
    Model sphere;
    Model island;
    Model rock;
    Model sea;
    Model dolphin;
    Model table;
    Model bird;
    Model toothless;
    
    Camera camera;

    Shader activeShader;
	Shader shader;
	Shader pbrShader;
    Shader skyboxShader;

    float deltaTime = 0.0f;

	glm::vec3 lightPosition{ -20.0f, 20.0f, 20.0f };

	glm::vec3 movingLightPosition{ 3.0f, 5.0f, 15.0f };

	glm::vec3 movingLightColor{ 1.0f, 0.0f, 0.0f };

	glm::vec3 ambientColor{ 0.1f, 0.1f, 0.1f };
	glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
	glm::vec3 directionalLightColor{ 1.0f, 1.0f, 1.0f };

	float movingLightRotation = 1.0f;

	float shininess = 128.0f;

	unsigned int skyboxVAO;

    unsigned int cubemapTexture;
    

	glm::vec3 lightPositions[4] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

    bool toggleDiffuse = false;
    bool toggleSpecular = false;
    bool toggleToonShading = false;
	bool toggleSpotLight = false;
	bool toggleFlatShading = false;
	bool toggleFog = false;
    bool toggleAnimation = false;
    bool togglePBR = false;
    bool toogleShadow = true;

private:
    void drawMovingLight();
public:
    sceneobjects_uniform( );

    void initScene();
    void update( float t );

    void renderShadowmap( void );
    void render();
    void resize(int, int);

    void onMouseMove(float xOffset, float yOffset) override;

	void keyPress(int key) override;

    void keyDown(int key) override;


    void initShadowmap();
};

#endif // SCENEBASIC_UNIFORM_H
