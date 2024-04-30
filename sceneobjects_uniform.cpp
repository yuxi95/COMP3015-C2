#include "sceneobjects_uniform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/scenerunner.h"
#include "helper/glutils.h"

SceneRunner* SceneRunner::singleton;

#include <GLFW/glfw3.h>

#define KEY_W                  87
#define KEY_S                  83
#define KEY_A                  65
#define KEY_D                  68
#define KEY_Q                  81
#define KEY_E                  69

#define KEY_1                  49
#define KEY_2                  50
#define KEY_3                  51
#define KEY_4                  52
#define KEY_5                  53
#define KEY_6                  54
#define KEY_7                  55
#define KEY_8                  56
#define KEY_9                  57
#define KEY_SPACE              32


using glm::vec3;

sceneobjects_uniform::sceneobjects_uniform() : angle(0.0f)
, camera({ 2.25438452 , 44.3377953 ,96.2507477 }, { 0., 1., 0. }, -90.1998825, -27.0999966)

{}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 4);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;



GLuint          def_normal_texture = 0;

void sceneobjects_uniform::initShadowmap() {

	const char* proj_texture_fn = "media\\models\\textures\\logo gouvernaille.png";
	const char* def_normal_fn = "media\\textures\\normal_mapping.png";

	// create projective texture 
	int width, height, nrComponents;


	unsigned char* data = stbi_load(proj_texture_fn, &width, &height, &nrComponents, 4);
	if (data)
	{
		glGenTextures(1, &proj_texture);
		glBindTexture(GL_TEXTURE_2D, proj_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		stbi_image_free(data);

	}
	else {
		std::cout << "texture failed to load at path: " << proj_texture_fn << std::endl;
		stbi_image_free(data);
		exit(1);
	}

	// default normal map
	data = stbi_load(def_normal_fn, &width, &height, &nrComponents, 4);
	if (data)
	{
		glGenTextures(1, &def_normal_texture);
		glBindTexture(GL_TEXTURE_2D, def_normal_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(data);

	}
	else {
		std::cout << "texture failed to load at path: " << def_normal_fn << std::endl;
		stbi_image_free(data);
		exit(1);
	}



	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);



	// glGenTextures(1, &color_texture);
	// glBindTexture(GL_TEXTURE_2D, color_texture);
	// glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 


	// The depth_FBO, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &depth_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

	// glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);


	glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	glReadBuffer(GL_NONE);

	// Always check that our depth_FBO is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "<!> failed to gen depth_FBO" << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depth_prog.load("shader/depth.vert", "shader/depth.frag");
	shadowmap_prog.load("shader/shadowmap.vert", "shader/shadowmap.frag");
	screen_prog.load("shader/rt.vert", "shader/rt.frag");

	screen_prog.use();
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glActiveTexture(GL_TEXTURE0);
	screen_prog.setInt("diffuse", 0);

	glfwSetWindowTitle(SceneRunner::singleton->window, "SCORE: 0");

}



void sceneobjects_uniform::initScene()
{
	initShadowmap();

	glEnable(GL_DEPTH_TEST);

	camera.MovementSpeed *= 10.0f;

	//toothless.load("media/models/toothless.obj");
	//bird.load("media/models/bird.obj");
	house.load("media/models/House.obj");
	sphere.load("media/models/sphere.obj");
	island.load("media/models/island.obj");
	rock.load("media/models/rock.obj");
	sea.load("media/models/sea.obj");
	table.load("media/models/Plane.obj");

	dolphin.load("media/models/dolphin.obj");


	auto stage_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f));
	auto tabble_model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 10.0f)), glm::vec3(5.0f));


	scene_objects.push_back(&house); 	house.model_transfrom = stage_model;
	scene_objects.push_back(&island); 	island.model_transfrom = stage_model;
	scene_objects.push_back(&rock); 	rock.model_transfrom = stage_model;
	scene_objects.push_back(&sea); 		sea.model_transfrom = stage_model;
	scene_objects.push_back(&table); 	table.model_transfrom = tabble_model;
	scene_objects.push_back(&dolphin);	dolphin.model_transfrom;

	scene_objects.push_back(&sphere);

	shader.load("shader/model_loading.vert", "shader/model_loading.frag");
	pbrShader.load("shader/pbr.vert", "shader/pbr.frag");
	skyboxShader.load("shader/skybox.vert", "shader/skybox.frag");

	activeShader = shader;

	//compile();

	//std::cout << std::endl;

	//prog.printActiveUniforms();

	///////////////////// Create the VBO ////////////////////
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces
	{
		"media/textures/skybox/CloudyCrown_Midday_Right.png",
		"media/textures/skybox/CloudyCrown_Midday_Left.png",
		"media/textures/skybox/CloudyCrown_Midday_Up.png",
		"media/textures/skybox/CloudyCrown_Midday_Down.png",
		"media/textures/skybox/CloudyCrown_Midday_Front.png",
		"media/textures/skybox/CloudyCrown_Midday_Back.png",
	};

	cubemapTexture = loadCubemap(faces);
}


float frand() {
	return (float(rand()) / RAND_MAX) * 2.f - 1.f;
}

float frand_1() {
	return (float(rand()) / RAND_MAX);
}

void sceneobjects_uniform::update(float t)
{
	deltaTime = t;
	treasure_timer -= t;


	// found treasure
	if (glm::distance(glm::vec2(player_pos.x, player_pos.z), glm::vec2(treasure_pos.x, treasure_pos.z)) < 5.f) {
		score += 100 + treasure_timer * 100;
		glfwSetWindowTitle(SceneRunner::singleton->window, (std::string("Score: ") + std::to_string(score)).c_str());
		treasure_timer = 0.f;
	}


	//reset treasure pos
	if (treasure_timer <= 0.f) {

		glm::mat3 rm = glm::rotate(glm::mat4(1.f), glm::radians(360 * frand()), { 0.f, 1.f, 0.f });

		treasure_pos = rm * glm::vec3(20.f + frand_1() * 30, 0, 0);

		table.model_transfrom = glm::translate(glm::mat4(1.f), treasure_pos) * glm::scale(glm::mat4(1.f), vec3(2.f));

		treasure_timer = 2 + frand();
	}

	const float pmove_speed = 40;
	// player controler
	if (glm::length2(player_mov)) {

		auto dmov = glm::normalize(player_mov);


		player_rm = glm::rotate(glm::mat4(1.0), glm::radians(-90.f), { 0.f,1.f, 0.f }) * glm::inverse(glm::lookAt({ 0.f,0.f,0.f }, dmov, { 0.f,1.f, 0.f }));

		auto dvec = pmove_speed * dmov * deltaTime;
		player_pos += dvec;


	}
	glm::mat4 sm = glm::scale(glm::mat4(1.0), glm::vec3(0.5f));
	glm::mat4 tm = glm::translate(glm::mat4(1.0), player_pos);
	dolphin.model_transfrom = tm * player_rm * sm;

	//update your angle here
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);

	auto model = glm::mat4(1.0f);

	activeShader.use();

	activeShader.setMat4("model", model);
	activeShader.setMat4("view", view);
	activeShader.setMat4("projection", projection);

	if (toogleShadow) {

		static float angle = 0.f;
		angle = angle + (100.f * deltaTime);

		glm::mat4 rm = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3{ 0.f,1.f,0.f });
		vec3 epos = rm * glm::vec4(-20.0f, 20.0f, -20.0f, 1.f);
		light_dir = glm::vec3(0.0f, 0.0f, 0.0f) - epos;

		// std::cout << angle << " " <<  glm::to_string(epos) << std::endl;


		float near_plane = 5.0f, far_plane = 100.f;
		glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(epos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		shadowmap_viewproj = lightProjection * lightView;


		activeShader = shadowmap_prog;

		activeShader.setVec3("camPos", camera.Position);
		activeShader.setVec3("LightDir", light_dir);




	}
	else if (togglePBR)
	{
		activeShader = pbrShader;

		activeShader.use();

		activeShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
		activeShader.setFloat("ao", 1.0f);

		activeShader.setVec3("camPos", camera.Position);

		activeShader.setFloat("metallic", 0.5f);
		activeShader.setFloat("roughness", glm::clamp(0.9f, 0.05f, 1.0f));

		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			activeShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			activeShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
		}
	}
	else
	{
		activeShader = shader;

		activeShader.use();

		activeShader.setVec3("lightPosition", lightPosition);
		activeShader.setVec3("movingLightPosition", movingLightPosition);
		activeShader.setVec3("ambientColor", ambientColor);
		activeShader.setVec3("lightColor", lightColor);
		activeShader.setVec3("movingLightColor", movingLightColor);
		activeShader.setVec3("directionalLightColor", directionalLightColor);
		activeShader.setVec3("viewPosition", camera.Position);
		activeShader.setFloat("shininess", shininess);
		activeShader.setBool("toggleDiffuse", toggleDiffuse);
		activeShader.setBool("toggleSpecular", toggleSpecular);
		activeShader.setBool("toggleToonShading", toggleToonShading);
		activeShader.setBool("toggleSpotLight", toggleSpotLight);
		activeShader.setBool("toggleFlatShading", toggleFlatShading);
		activeShader.setBool("toggleFog", toggleFog);

		activeShader.setVec3("spotLight.position", camera.Position);
		activeShader.setVec3("spotLight.direction", camera.Front);
		activeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		activeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		activeShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
		activeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		activeShader.setFloat("spotLight.constant", 1.0f);
		activeShader.setFloat("spotLight.linear", 0.09f);
		activeShader.setFloat("spotLight.quadratic", 0.032f);
	}

	if (toggleAnimation)
	{
		auto rx0 = 0.0f;
		auto rz0 = 20.0f;

		auto x = movingLightPosition.x;
		auto z = movingLightPosition.z;

		auto a = glm::radians(movingLightRotation);

		auto x0 = (x - rx0) * std::cos(a) - (z - rz0) * std::sin(a) + rx0;
		auto z0 = (x - rx0) * std::sin(a) + (z - rz0) * std::cos(a) + rz0;

		movingLightPosition.x = x0;
		movingLightPosition.z = z0;
	}


	player_mov = { 0.f, 0.f,0.f };
}

void sceneobjects_uniform::drawMovingLight()
{
	auto model = glm::translate(glm::mat4(1.0f), movingLightPosition);
	model = glm::scale(model, { 0.5f, 0.5f, 0.5f });

	activeShader.setMat4("model", model);

	activeShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

	sphere.Draw(activeShader);

	model = glm::translate(glm::mat4(1.0f), lightPosition);

	activeShader.setMat4("model", model);

	activeShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

	sphere.Draw(activeShader);
}


void sceneobjects_uniform::renderShadowmap() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);

	glClear(GL_DEPTH_BUFFER_BIT);


	depth_prog.use();
	glm::mat4 mvp;
	for (auto object : scene_objects) {

		mvp = shadowmap_viewproj * object->model_transfrom;
		depth_prog.setMat4("lightSpaceMatrix", mvp);
		object->DrawDepth(depth_prog);
	}

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glDisable(GL_CULL_FACE); // don't forget to reset original culling face
	glCullFace(GL_BACK);

}

void sceneobjects_uniform::render()
{
	if (toogleShadow) {
		renderShadowmap();

	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	activeShader.use();

	if (toogleShadow) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_texture);
		activeShader.setInt("shdowMap", 0);


		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, proj_texture);
		activeShader.setInt("projMap", 1);
	}


	glm::mat4 lsm;
	for (auto object : scene_objects) {
		if (toogleShadow) {
			lsm = shadowmap_viewproj * object->model_transfrom;
			activeShader.setMat4("lightSpaceMatrix", lsm);
		}

		activeShader.setMat4("model", object->model_transfrom);
		activeShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(object->model_transfrom))));

		object->Draw(activeShader);
	}


	//drawMovingLight();

	glBindVertexArray(0);

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.use();
	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default


	if (toogleShadow) {
		glViewport(8, 8, 128, 128);
		screen_prog.use();

		glBindVertexArray(skyboxVAO);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	}

}

void sceneobjects_uniform::resize(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

void sceneobjects_uniform::onMouseMove(float xOffset, float yOffset)
{
	camera.ProcessMouseMovement(xOffset, yOffset);
}

static vec3 player_mov;

void sceneobjects_uniform::keyPress(int key)
{


	switch (key)
	{
	case KEY_W:
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case KEY_S:
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case KEY_A:
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case KEY_D:
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	case KEY_Q:
		camera.ProcessKeyboard(DOWN, deltaTime);
		break;
	case KEY_E:
		camera.ProcessKeyboard(UP, deltaTime);
		break;

	case GLFW_KEY_UP:
		player_mov.z -= 1.f;
		break;
	case GLFW_KEY_DOWN:
		player_mov.z += 1.f;
		break;
	case GLFW_KEY_LEFT:
		player_mov.x -= 1.f;
		break;
	case GLFW_KEY_RIGHT:
		player_mov.x += 1.f;
		break;


	default:
		break;


	}
}

void sceneobjects_uniform::keyDown(int key)
{
	switch (key)
	{
	case KEY_1:
		toggleDiffuse = !toggleDiffuse;
		break;
	case KEY_2:
		toggleSpecular = !toggleSpecular;
		break;
	case KEY_3:
		toggleSpotLight = !toggleSpotLight;
		break;
	case KEY_4:
		toggleToonShading = !toggleToonShading;
		break;
	case KEY_5:
		toggleFlatShading = !toggleFlatShading;
		break;
	case KEY_6:
		toggleFog = !toggleFog;
		break;
	case KEY_7:
		togglePBR = !togglePBR;
		break;

	case KEY_8:
		toogleShadow = !toogleShadow;
		break;

	case KEY_SPACE:
		toggleAnimation = !toggleAnimation;
		break;
	default:
		break;
	}
}
