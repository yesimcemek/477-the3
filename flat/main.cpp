#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int xpos, ypos;
glm::vec3 cameraPosition;
glm::vec3 lightPosition;
glm::vec3 cameraFront    = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp       = glm::vec3(0.0f, 1.0f, 0.0f);
const static float aspect_ratio = 1.0f;

int widthTexture, heightTexture;
unsigned int VBO, VAO, EBO;

float heightFactor = 10.0;
float speed = 0.0;
bool fullScreen = false;
float pitch = 0.0f;
float yaw =  90.0f;
int textureOffset = 0;

static void errorCallback(int error,
const char * description) {
fprintf(stderr, "Error: %s\n", description);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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
					fullScreen = true;
				glfwGetWindowPos(window, &xpos, &ypos);
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 0);

				}
			}
			break;
		case GLFW_KEY_ESCAPE:
			if(action == GLFW_PRESS){
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			break;
		case GLFW_KEY_R:
			heightFactor += 0.5;
			break;
		case GLFW_KEY_F:
			heightFactor -= 0.5;
			break;
		case GLFW_KEY_Y:
				speed += 0.01;
				break;
		case GLFW_KEY_H:
				speed -= 0.01;
				break;
		case GLFW_KEY_T:
				lightPosition.y += 5;
				break;
		case GLFW_KEY_G:
				lightPosition.y -= 5;
				break;
		case GLFW_KEY_X:
				speed = 0.0;
				break;
		case GLFW_KEY_W:
				pitch += 0.05f;
				if(pitch == 90.0f)
						pitch = 90.05f;
				if(pitch >= 270.0f)
						pitch = -89.95f;
				if(pitch == -90.0f)
						pitch += 0.05f;

				if(pitch >= -90.0f and pitch <= 90.0f)
						cameraUp = glm::vec3(0, 1, 0);
				else
						cameraUp = glm::vec3(0, -1, 0);
				break;
		case GLFW_KEY_S:
				pitch -= 0.05f;
				if(pitch == -90.0f)
						pitch = -90.05f;
				if(pitch <= -270.0f)
						pitch = 90.05f;

				if(pitch >= -90.0f and pitch <= 90.0f)
						cameraUp = glm::vec3(0, 1, 0);
				else
						cameraUp = glm::vec3(0, -1, 0);

				break;
		case GLFW_KEY_A:
				yaw -= 0.05f;
				break;
		case GLFW_KEY_D:
				yaw += 0.05f;
				break;
		case GLFW_KEY_Q:
				textureOffset += 1;
				break;
		case GLFW_KEY_E:
				textureOffset -= 1;
				break;
		case GLFW_KEY_I:
				cameraPosition = glm::vec3(widthTexture/2.0f, widthTexture/10.0f, -widthTexture/4.0f);
				speed = 0.0f;
				pitch = 0.0f;
				yaw =  90.0f;
				heightFactor = 10.0;
				speed = 0.0;
				textureOffset = 0;
				break;
		default:
				break;
	}
	if(key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_I)
	{

			glm::vec3 newFront;
			newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			newFront.y = sin(glm::radians(pitch));
			newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(newFront);
	}
}

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Only one texture image expected!\n");
		exit(-1);
	}

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	win = glfwCreateWindow(1000, 1000, "CENG477 - HW3", NULL, NULL);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

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
	initTexture(argv[1], & widthTexture, & heightTexture);

	int vertexCount = (widthTexture + 1) * (heightTexture + 1);
	float *vertices = new float[vertexCount * 3];

	int vertexArrayPosition = 0;
	for(int coordinateZ = 0; coordinateZ <= heightTexture; coordinateZ++)
	{
		for(int coordinateX = 0; coordinateX <= widthTexture; coordinateX++)
		{
			vertices[vertexArrayPosition] = coordinateX;
			vertices[vertexArrayPosition+1] = 0;
			vertices[vertexArrayPosition+2] = coordinateZ;
			vertexArrayPosition += 3;
		}
	}

	int indexCount = 6 * widthTexture * heightTexture;
    unsigned int* indices = new unsigned int[indexCount];
    int indexArrayPosition = 0;
    for(int z=0; z < heightTexture; z++)
    {
    	for(int x=0; x < widthTexture; x++)
    	{
			int startIndex = x + z*(widthTexture+1);
			int endIndex = (x + 1) + (z + 1)*(widthTexture+1);

	    	indices[indexArrayPosition] = startIndex;
	    	indices[indexArrayPosition + 1] = endIndex - 1;
	    	indices[indexArrayPosition + 2] = startIndex + 1;

    		indices[indexArrayPosition + 3] = startIndex + 1;
	    	indices[indexArrayPosition + 4] = endIndex - 1;
	    	indices[indexArrayPosition + 5] = endIndex;
	    	indexArrayPosition += 6;
    	}
    }

	cameraPosition = glm::vec3(widthTexture/2.0f, widthTexture/10.0f, -widthTexture/4.0f);
	lightPosition = glm::vec3(widthTexture/2.0f, 100.0f, heightTexture/2.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

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
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glm::mat4 view;
		glm::mat4 projection;

		unsigned int widthLoc = glGetUniformLocation(idProgramShader, "widthTexture");
		unsigned int heightLoc  = glGetUniformLocation(idProgramShader, "heightTexture");
		unsigned int heightFactorLoc  = glGetUniformLocation(idProgramShader, "heightFactor");
		unsigned int textureOffsetLoc  = glGetUniformLocation(idProgramShader, "textureOffset");

		glUniform1i(textureOffsetLoc, textureOffset%widthTexture);
		glUniform1i(widthLoc, widthTexture);
		glUniform1i(heightLoc, heightTexture);
		glUniform1f(heightFactorLoc, heightFactor);

		cameraPosition += speed * cameraFront;
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

		projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);

		// retrieve the matrix uniform locations
		unsigned int viewLoc  = glGetUniformLocation(idProgramShader, "view");
		unsigned int projLoc  = glGetUniformLocation(idProgramShader, "projection");
		unsigned int lightPosLoc  = glGetUniformLocation(idProgramShader, "lightPosition");
		unsigned int cameraPosLoc  = glGetUniformLocation(idProgramShader, "cameraPosition");

		// pass them to the shaders (3 different ways)
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
		glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
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
