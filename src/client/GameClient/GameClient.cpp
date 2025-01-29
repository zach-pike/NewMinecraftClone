#include "GameClient.hpp"

#include <functional>

#include "GL/ShaderLoaders/VertexFragment.hpp"

#include "Common/PacketType.hpp"
#include "Common/UpdatePlayerState/UpdatePlayerState.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

static const GLfloat playerVertexData[] = { 
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
};

static const GLfloat playerUVData[] = { 
	0.000059f, 1.0f-0.000004f, 
	0.000103f, 1.0f-0.336048f, 
	0.335973f, 1.0f-0.335903f, 
	1.000023f, 1.0f-0.000013f, 
	0.667979f, 1.0f-0.335851f, 
	0.999958f, 1.0f-0.336064f, 
	0.667979f, 1.0f-0.335851f, 
	0.336024f, 1.0f-0.671877f, 
	0.667969f, 1.0f-0.671889f, 
	1.000023f, 1.0f-0.000013f, 
	0.668104f, 1.0f-0.000013f, 
	0.667979f, 1.0f-0.335851f, 
	0.000059f, 1.0f-0.000004f, 
	0.335973f, 1.0f-0.335903f, 
	0.336098f, 1.0f-0.000071f, 
	0.667979f, 1.0f-0.335851f, 
	0.335973f, 1.0f-0.335903f, 
	0.336024f, 1.0f-0.671877f, 
	1.000004f, 1.0f-0.671847f, 
	0.999958f, 1.0f-0.336064f, 
	0.667979f, 1.0f-0.335851f, 
	0.668104f, 1.0f-0.000013f, 
	0.335973f, 1.0f-0.335903f, 
	0.667979f, 1.0f-0.335851f, 
	0.335973f, 1.0f-0.335903f, 
	0.668104f, 1.0f-0.000013f, 
	0.336098f, 1.0f-0.000071f, 
	0.000103f, 1.0f-0.336048f, 
	0.000004f, 1.0f-0.671870f, 
	0.336024f, 1.0f-0.671877f, 
	0.000103f, 1.0f-0.336048f, 
	0.336024f, 1.0f-0.671877f, 
	0.335973f, 1.0f-0.335903f, 
	0.667969f, 1.0f-0.671889f, 
	1.000004f, 1.0f-0.671847f, 
	0.667979f, 1.0f-0.335851f
};


glm::vec3 getLookingVector(float pitch, float yaw) {
    return glm::vec3(cos(pitch)*cos(yaw), sin(pitch), sin(yaw) * cos(pitch));
}

void GameClient::_renderThread() {
    // Initialize OpenGL and load relevant extensions
    if (!glfwInit()) {
        throw std::runtime_error("Failed to init glfw!");
    }

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Window", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL/GLAD!\n";
        std::exit(1);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glClearColor(.25f, .5f, .75f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const char* vendor = (char*)glGetString(GL_VENDOR); // Returns the vendor
    const char* renderer = (char*)glGetString(GL_RENDERER); // Returns a hint to the model
    printf("Vendor: %s\nRenderer: %s\n", vendor, renderer);

    // OpenGL is ready!

    // VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // IMGui state
    char hostIP[20] = "127.0.0.1";
    int port = 7777;

    srand(time(nullptr));

    auto crand = []() {
        return (float)rand() / (float)RAND_MAX;
    };

    playerData.playerPosition = glm::vec3(crand()*10-5, 0, crand()*10-5);

    playerData.cameraPitch = 0;
    playerData.cameraYaw = 0;

    auto playerShader = loadVertexFragmentShader("shader/player/");
    playerShader->use();

    GLuint mvpUniform = playerShader->getUniformLocation("MVP");
    GLuint positionUniform = playerShader->getUniformLocation("position");

    GLuint playerVertexBuffer;
    glGenBuffers(1, &playerVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, playerVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertexData), playerVertexData, GL_STATIC_DRAW);

    GLuint playerUVBuffer;
    glGenBuffers(1, &playerUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, playerUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerUVData), playerUVData, GL_STATIC_DRAW);

    float aspectRatio;

    double lastMouseX, lastMouseY;

    const float mouseSens = .005f;
    const float moveSpeed = 0.01f;

    bool mouseLocked = false;
    bool mouseLockKeyPressed = false;

    int fc = 0;

    // Main render loop
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        // Do network loop
        while(networkClient.messagesAvailable()) {
            auto message = networkClient.popMessage();

            assert(message->dataLength > 0);
            PacketType pt = *(PacketType*)message->data;

            switch(pt) {
                case PacketType::UpdatePlayerState: {
                    UpdatePlayerState ups;
                    ups.decodePacket(message);
                    
                    players[ups.userToUpdate] = ups.playerState;
                } break;
            }
        }

        if (fc % 20 == 0) {
            ENetPacket* p = playerData.convToPacket();
            networkClient.addToOutQueue(p);
        }

        if (!renderThreadRunning) break;
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mouse lock key
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (!mouseLockKeyPressed) {
                mouseLocked = !mouseLocked;

                if (mouseLocked) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }

                mouseLockKeyPressed = true;
            }
        } else {
            mouseLockKeyPressed = false;
        }

        // Movement keys
        if (mouseLocked) {
            glm::vec3 movement = glm::vec3(0, 0, 0);
            auto looking = getLookingVector(playerData.cameraPitch, playerData.cameraYaw);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                movement += looking;
            }

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                movement += -looking;
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                movement += glm::vec3(looking.z, 0, -looking.x);
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                movement += -glm::vec3(looking.z, 0, -looking.x);
            }

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                movement += glm::vec3(0, 1, 0);
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                movement -= glm::vec3(0, 1, 0);
            }

            if (glm::length(movement) > 0.5f) {
                playerData.playerPosition += glm::normalize(movement) * moveSpeed;
            }
        }

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        if (windowWidth != 0 && windowHeight != 0) {
            aspectRatio = (float)windowWidth / (float)windowHeight;
        }

        if (mouseLocked) {
            double mousePosX, mousePosY;
            glfwGetCursorPos(window, &mousePosX, &mousePosY);

            double mouseDeltaX = mousePosX - lastMouseX;
            double mouseDeltaY = mousePosY - lastMouseY;

            lastMouseX = mousePosX;
            lastMouseY = mousePosY;

            playerData.cameraYaw = std::fmod(playerData.cameraYaw + mouseDeltaX * mouseSens, M_PI*2);

            // Lock the camera to almost all the way down and almost all the way up
            playerData.cameraPitch = std::min(1.5f*M_PI - .01, std::max((double)M_PI_2 + .01, playerData.cameraPitch + mouseDeltaY * mouseSens));
        }

        glm::mat4 model = glm::mat4(1);
        glm::mat4 view = glm::lookAt(
                                        playerData.playerPosition + glm::vec3(0, 2, 0), 
                                        playerData.playerPosition + glm::vec3(0, 2, 0) + getLookingVector(playerData.cameraPitch, playerData.cameraYaw),
                                        glm::vec3(0, 1, 0)
                                    );
        glm::mat4 projection = glm::perspective(glm::radians(70.f), aspectRatio, 0.01f, 100.f);

        glm::mat4 MVP = projection * view;

        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &MVP[0][0]);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, playerVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            playerShader->use();
        for (auto& player : players) {
            glBindBuffer(GL_ARRAY_BUFFER, playerUVBuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glUniform3fv(positionUniform, 1, &player.second.playerPosition[0]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Connection window");
            ImGui::InputText("Host IP", hostIP, 20);
            ImGui::InputInt("Host Port", &port);

            if (ImGui::Button("Connect!") && !networkClient.isConnected()) {
                networkClient.connectToHost(std::string(hostIP), port);
            } else if (networkClient.isConnected()) {
                ImGui::Text("Connected!");
            }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        fc++;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    renderThreadRunning = false;
}

GameClient::GameClient() {}
GameClient::~GameClient() {}

void GameClient::startGame() {
    renderThreadRunning = true;
    renderThread = std::thread(std::bind(&GameClient::_renderThread, this));
}

void GameClient::stopGame() {
    renderThreadRunning = false;
    renderThread.join();

    if (networkClient.isConnected()) {
        networkClient.disconnect();
    }
}

bool GameClient::isRenderThreadRunning() const {
    return renderThreadRunning;
}