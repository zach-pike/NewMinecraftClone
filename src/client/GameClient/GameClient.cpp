#include "GameClient.hpp"

#include <functional>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

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

    playerData.playerPosition = glm::vec3(1, 2, 3);
    playerData.cameraPitch = 69;
    playerData.cameraYaw = 420;

    // Main render loop
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        if (!renderThreadRunning) break;
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            if (ImGui::Button("Send Test Packet")) {
                ENetPacket* p = playerData.convToPacket();
                networkClient.addToOutQueue(p);
            }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
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