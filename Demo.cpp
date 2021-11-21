#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("multipleLight.vert", "multipleLight.frag", nullptr);
	BuildTvDepan();
	BuildTvSamping();
	BuildTvBelakang();
	BuildColoredPlane();
	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &EBO3);
	glDeleteVertexArrays(1, &VAO4);
	glDeleteBuffers(1, &VBO4);
	glDeleteBuffers(1, &EBO4);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);


}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 1.5f) / 1000);
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::vec3 cameraPos = glm::vec3(0, 3, 3);
	glm::vec3 cameraFront = glm::vec3(0, -1, -1);
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// set lighting attributes
	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.direction"), 0.0f, -1.0f, -1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	// point light 1
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].position"), 0.0f, 3.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].ambient"), 1.0f, 0.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].diffuse"), 1.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].specular"), 0.4f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].quadratic"), 0.032f);
	// point light 2
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].position"), -2.0f, 3.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].ambient"), 0.0f, 1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].diffuse"), 0.0f, 1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].specular"), 0.0f, 1.0f, 0.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].quadratic"), 0.032f);
	// point light 3
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].position"), 2.0f, 3.0f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].ambient"), 0.0f, 0.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].diffuse"), 0.0f, 0.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].specular"), 0.0f, 0.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].quadratic"), 0.032f);
	// point light 4
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].position"), 0.0f, 3.0f, 2.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].ambient"), 0.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].diffuse"), 0.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].specular"), 0.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].quadratic"), 0.032f);
	// spotLight
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLight.position"), 1, &cameraPos[0]);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLight.direction"), 1, &cameraFront[0]);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.ambient"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

	DrawTvDepan();
	DrawTvBelakang();
	DrawTvSamping();
	DrawColoredPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildTvDepan() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("TvDepan.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sTexture);
	glBindTexture(GL_TEXTURE_2D, sTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("TvDepan_Specular.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-1.5, -1, 0.5, 0, 0,0.0f,  0.0f,  1.0f, // 0
		1.5, -1, 0.5, 1, 0,0.0f,  0.0f,  1.0f,   // 1
		1.5,  1, 0.5, 1, 1, 0.0f,  0.0f,  1.0f,  // 2
		-1.5,  1, 0.5, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.5,  1,  0.5, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.5,  1, -0.5, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.5, -1, -0.5, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.5, -1,  0.5, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		1.5, 1, -0.5, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		-1.5,  1, -0.5, 1, 0, 0.0f,  0.0f,  -1.0f,// 9
		-1.5,   -1, -0.5, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		1.5,  -1, -0.5, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.5, -1, -0.5, 1, 1, -1.0f,  0.0f,  0.0f, // 12
		-1.5, -1,  0.5, 0, 1, -1.0f,  0.0f,  0.0f, // 13
		-1.5,  1,  0.5, 0, 0, -1.0f,  0.0f,  0.0f,// 14
		-1.5,  1, -0.5, 1, 0, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.5, 1,  0.5, 1, 1, 0.0f,  1.0f,  0.0f,  // 16
		-1.5, 1,  0.5, 0, 1,  0.0f,  1.0f,  0.0f, // 17
		-1.5, 1, -0.5, 0, 0,  0.0f,  1.0f,  0.0f, // 18
		1.5, 1, -0.5, 1, 0,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.5, -1, -0.5, 1, 1, 0.0f,  -1.0f,  0.0f, // 20
		1.5, -1, -0.5, 0, 1,   0.0f,  -1.0f,  0.0f,// 21
		1.5, -1,  0.5, 1, 1,  0.0f,  -1.0f,  0.0f,// 22
		-1.5, -1,  0.5, 0, 1, 0.0f,  -1.0f,  0.0f, // 23

	};


	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		//4,  5,  6,  4,  6,  7,   // right
		//8,  9,  10, 8,  10, 11,  // back
		//12, 14, 13, 12, 15, 14,  // left
		//16, 18, 17, 16, 19, 18,  // upper
		//20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Demo::BuildTvSamping() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("TvSamping.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sTexture3);
	glBindTexture(GL_TEXTURE_2D, sTexture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("TvSamping_Specular.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-1.5, -1, 0.5, 0, 0,0.0f,  0.0f,  1.0f, // 0
		1.5, -1, 0.5, 1, 0,0.0f,  0.0f,  1.0f,   // 1
		1.5,  1, 0.5, 1, 1, 0.0f,  0.0f,  1.0f,  // 2
		-1.5,  1, 0.5, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.5,  1,  0.5, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.5,  1, -0.5, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.5, -1, -0.5, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.5, -1,  0.5, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		1.5, 1, -0.5, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		-1.5,  1, -0.5, 1, 0, 0.0f,  0.0f,  -1.0f,// 9
		-1.5,   -1, -0.5, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		1.5,  -1, -0.5, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.5, -1, -0.5, 1, 1, -1.0f,  0.0f,  0.0f, // 12
		-1.5, -1,  0.5, 0, 1, -1.0f,  0.0f,  0.0f, // 13
		-1.5,  1,  0.5, 0, 0, -1.0f,  0.0f,  0.0f,// 14
		-1.5,  1, -0.5, 1, 0, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.5, 1,  0.5, 1, 1, 0.0f,  1.0f,  0.0f,  // 16
		-1.5, 1,  0.5, 0, 1,  0.0f,  1.0f,  0.0f, // 17
		-1.5, 1, -0.5, 0, 0,  0.0f,  1.0f,  0.0f, // 18
		1.5, 1, -0.5, 1, 0,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.5, -1, -0.5, 1, 1, 0.0f,  -1.0f,  0.0f, // 20
		1.5, -1, -0.5, 0, 1,   0.0f,  -1.0f,  0.0f,// 21
		1.5, -1,  0.5, 1, 1,  0.0f,  -1.0f,  0.0f,// 22
		-1.5, -1,  0.5, 0, 1, 0.0f,  -1.0f,  0.0f, // 23

	};


	unsigned int indices[] = {
		//0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		//8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
void Demo::BuildTvBelakang() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("TvBelakang.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sTexture4);
	glBindTexture(GL_TEXTURE_2D, sTexture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("TvBelakang_Specular.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front
		-1.5, -1, 0.5, 0, 0,0.0f,  0.0f,  1.0f, // 0
		1.5, -1, 0.5, 1, 0,0.0f,  0.0f,  1.0f,   // 1
		1.5,  1, 0.5, 1, 1, 0.0f,  0.0f,  1.0f,  // 2
		-1.5,  1, 0.5, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// right
		1.5,  1,  0.5, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		1.5,  1, -0.5, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		1.5, -1, -0.5, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		1.5, -1,  0.5, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		// back
		1.5, 1, -0.5, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		-1.5,  1, -0.5, 1, 0, 0.0f,  0.0f,  -1.0f,// 9
		-1.5,   -1, -0.5, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		1.5,  -1, -0.5, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-1.5, -1, -0.5, 1, 1, -1.0f,  0.0f,  0.0f, // 12
		-1.5, -1,  0.5, 0, 1, -1.0f,  0.0f,  0.0f, // 13
		-1.5,  1,  0.5, 0, 0, -1.0f,  0.0f,  0.0f,// 14
		-1.5,  1, -0.5, 1, 0, -1.0f,  0.0f,  0.0f, // 15

		// upper
		1.5, 1,  0.5, 1, 1, 0.0f,  1.0f,  0.0f,  // 16
		-1.5, 1,  0.5, 0, 1,  0.0f,  1.0f,  0.0f, // 17
		-1.5, 1, -0.5, 0, 0,  0.0f,  1.0f,  0.0f, // 18
		1.5, 1, -0.5, 1, 0,   0.0f,  1.0f,  0.0f, // 19

		// bottom
		-1.5, -1, -0.5, 1, 1, 0.0f,  -1.0f,  0.0f, // 20
		1.5, -1, -0.5, 0, 1,   0.0f,  -1.0f,  0.0f,// 21
		1.5, -1,  0.5, 1, 1,  0.0f,  -1.0f,  0.0f,// 22
		-1.5, -1,  0.5, 0, 1, 0.0f,  -1.0f,  0.0f, // 23

	};


	unsigned int indices[] = {
		//0,  1,  2,  0,  2,  3,   // front
		//4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		//12, 14, 13, 12, 15, 14,  // left
		//16, 18, 17, 16, 19, 18,  // upper
		//20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawTvDepan()
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sTexture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 0.4f);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, 3, 0));

	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(3, 3, 3));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
void Demo::DrawTvBelakang()
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sTexture4);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 0.4f);

	glBindVertexArray(VAO4);// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, 3, 0));

	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(3, 3, 3));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawTvSamping()
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sTexture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 0.4f);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized



	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, 3, 0));

	model = glm::rotate(model, angle, glm::vec3(0, 1, 0));

	model = glm::scale(model, glm::vec3(3, 3, 3));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildColoredPlane()
{

	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("marble.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sTexture2);
	glBindTexture(GL_TEXTURE_2D, sTexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("Marble_Specularmap.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -1.5, -50.0,  0,  0,
		 50.0, -1.5, -50.0, 50,  0,
		 50.0, -1.5,  50.0, 50, 50,
		-50.0, -1.5,  50.0,  0, 50,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}
void Demo::DrawColoredPlane()
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sTexture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 0.4f);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.002f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}
void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}
void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Edit Rotation & Camera", 800, 600, false, false);
}
