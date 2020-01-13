#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <math.h>
#include <vector>

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

int xpos, ypos;
glm::vec3 cameraPosition;
glm::vec3 lightPosition;
glm::vec3 cameraFront    = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 cameraUp       = glm::vec3(0.0f, 0.0f, 1.0f);
const static float aspect_ratio = 1.0f;

int widthTexture, heightTexture;
int widthMap, heightMap;
unsigned int VBO, VAO, EBO;

float heightFactor = 0;
float speed = 0.0;
bool fullScreen = false;
float pitch = 0.0f;
float yaw = 90.0f;
int textureOffset = 0;

static void errorCallback(int error,
const char * description) {
fprintf(stderr, "Error: %s\n", description);
}

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods ){
	int width, height;
	switch (key) {
		case GLFW_KEY_P:
		std::cout << action << std::endl;
		if(action == GLFW_PRESS){
			if(fullScreen){
			glfwSetWindowMonitor(window, NULL, xpos, ypos, 1000, 1000, 0);
			fullScreen = false;
			}
			else{
			glfwGetWindowPos(window, &xpos, &ypos);
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), xpos, ypos, 1920, 1080, 0);
			fullScreen = true;
			}
		}
		break;
		case GLFW_KEY_ESCAPE:
		if(action == GLFW_PRESS){
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		break;
		case GLFW_KEY_R:
		if(action == GLFW_PRESS){
			heightFactor += 0.5;
		}
		break;
		case GLFW_KEY_F:
		if(action == GLFW_PRESS){
			heightFactor -= 0.5;
		}
		break;
		case GLFW_KEY_Y:      // Increases speed constant
			speed += 0.01;
			break;
		case GLFW_KEY_H:      // Decreases speed constant
			speed -= 0.01;
			break;
		case GLFW_KEY_X:
			speed = 0.0;
			break;
		case GLFW_KEY_W:
				pitch += 0.05f;
				break;
		case GLFW_KEY_S:
				pitch -= 0.05f;
				break;
		case GLFW_KEY_A:
				yaw -= 0.05f;
				break;
		case GLFW_KEY_D:
				yaw += 0.05f;
				break;
		case GLFW_KEY_Q:
				textureOffset -= 1;
				break;
		case GLFW_KEY_E:
				textureOffset += 1;
				break;
		case GLFW_KEY_I:
			cameraPosition = glm::vec3(0, 600, 0);
			lightPosition = glm::vec3(0, 1600.0f, 0);
			speed = 0.0f;
			heightFactor = 10.0;
			pitch = 0.0f;
			yaw = 90.0f;
			break;
		default:
			break;
	}
	if(key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_I)
	{

		glm::vec3 gaze;
		gaze.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		gaze.y = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		gaze.z = sin(glm::radians(pitch));
		cameraFront = glm::normalize(gaze);
	}
}

int main(int argc, char * argv[]) {

	if (argc != 3) {
		printf("Only one texture image expected!\n");
		exit(-1);
	}

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	win = glfwCreateWindow(1000, 1000, "CENG477 - HW3", NULL, NULL);
	//glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

	if (!win) {
		glfwTerminate();
		exit(-1);
	}

	//glfwSetKeyCallback(win, callback_key);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_callback);
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		glfwTerminate();
		exit(-1);
	}

	initShaders();
	glUseProgram(idProgramShader);
	initTexture(argv[1], & widthMap, & heightMap, idHeightTexture, 0);
	initTexture(argv[2], & widthTexture, & heightTexture, idJpegTexture, 1);

	int horizontalSplitCount = 250;
	int verticalSplitCount = 125;
	float radius = 175;
	int vertexCount = (horizontalSplitCount + 1) * (verticalSplitCount + 1);
	float *vertices = new float[vertexCount * 3];
	float beta = 0;
	float alpha = 0;

	int vertexArrayPosition = 0;
	for(int verticalStep = 0; verticalStep <= verticalSplitCount; verticalStep++)
	{
		beta = M_PI * ((float)verticalStep / verticalSplitCount);

		for(int horizontalStep = 0; horizontalStep <= horizontalSplitCount; horizontalStep++)
		{
			alpha = 2 * M_PI * ((float)horizontalStep / horizontalSplitCount);

			vertices[vertexArrayPosition] = radius * sin(beta) * cos(alpha);
			vertices[vertexArrayPosition+1] = radius * sin(beta) * sin(alpha);
			vertices[vertexArrayPosition+2] = radius * cos(beta);
			vertexArrayPosition += 3;
		}
	}



	//int indexCount = 2 * vertexCount;
	int indexCount = 0;
	for(int verticalStep = 0; verticalStep <= verticalSplitCount; verticalStep++)
	{
		for(int horizontalStep = 0; horizontalStep <= horizontalSplitCount; horizontalStep++)
		{
            indexCount += 6;
		}
	}

	unsigned int* indices = new unsigned int[indexCount];
	unsigned int startIndex = 0;
	unsigned int endIndex = 0;
	int indexArrayPosition = 0;
	for(int verticalStep = 0; verticalStep <= verticalSplitCount; verticalStep++)
	{
		for(int horizontalStep = 0; horizontalStep <= horizontalSplitCount; horizontalStep++)
		{
			startIndex = verticalStep * horizontalSplitCount + horizontalStep;
			endIndex = (verticalStep + 1) * horizontalSplitCount + (horizontalStep + 1);

			indices[indexArrayPosition] = startIndex;
			indices[indexArrayPosition + 1] = endIndex - 1;
			indices[indexArrayPosition + 2] = startIndex + 1;

			indices[indexArrayPosition + 3] = startIndex + 1;
			indices[indexArrayPosition + 4] = endIndex - 1;
			indices[indexArrayPosition + 5] = endIndex;

			indexArrayPosition += 6;
		}
	}

	cameraPosition = glm::vec3(0, 600, 0);
	lightPosition = glm::vec3(0, 1600.0f, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 3 * vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

	glUseProgram(idProgramShader);

	while (!glfwWindowShouldClose(win)) {

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, idJpegTexture);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, idHeightTexture);

		glm::mat4 view;
		glm::mat4 projection;
		float offset = (textureOffset % 250)/250.0f;

		unsigned int widthLoc = glGetUniformLocation(idProgramShader, "widthTexture");
		unsigned int heightLoc  = glGetUniformLocation(idProgramShader, "heightTexture");
		unsigned int heightFactorLoc  = glGetUniformLocation(idProgramShader, "heightFactor");
		unsigned int radiusLoc  = glGetUniformLocation(idProgramShader, "radius");
		unsigned int piLoc  = glGetUniformLocation(idProgramShader, "pi");
		unsigned int textureOffsetLoc = glGetUniformLocation(idProgramShader, "textureOffset");

		
		glUniform1i(widthLoc, widthTexture);
		glUniform1i(heightLoc, heightTexture);
		glUniform1f(heightFactorLoc, heightFactor);
		glUniform1f(radiusLoc, radius);
		glUniform1f(piLoc, M_PI);
		glUniform1f(textureOffsetLoc, offset);


		cameraPosition += speed * cameraFront;
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

		projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);

		// retrieve the matrix uniform locations
		unsigned int viewLoc  = glGetUniformLocation(idProgramShader, "view");
		unsigned int projLoc  = glGetUniformLocation(idProgramShader, "projection");
		unsigned int lightPosLoc  = glGetUniformLocation(idProgramShader, "lightPosition");
		unsigned int cameraPosLoc  = glGetUniformLocation(idProgramShader, "cameraPosition");

		unsigned int heightTexLoc = glGetUniformLocation(idProgramShader, "heightTex");
		unsigned int normalTexLoc  = glGetUniformLocation(idProgramShader, "normalTex");

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
		glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);

		glUniform1i(heightTexLoc, 0);
		glUniform1i(normalTexLoc, 1);
		// render container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwDestroyWindow(win);
	glfwTerminate();

	return 0;
}
