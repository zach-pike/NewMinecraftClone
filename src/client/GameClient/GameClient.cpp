#include "GameClient.hpp"

#include <functional>

#include "GL/ShaderLoaders/VertexFragment.hpp"

#include "Common/Packets/PacketType.hpp"
#include "Common/Packets/UpdatePlayerState/UpdatePlayerState.hpp"
#include "Common/Packets/ChunkData/ChunkData.hpp"
#include "Common/Packets/ChunkUpdate/ChunkUpdate.hpp"

#include "ChunkManager/AABB/AABB.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <algorithm>

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

template <typename T>
static T mod(T a, T b) {
    return (a % b + b) % b;
}


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

    playerData.playerPosition = glm::vec3(0, 0, 0);

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
    const float moveSpeed = 0.18f;
    float physSpeed = 0.18f;

    bool mouseLocked = false;
    bool mouseLockKeyPressed = false;

    int fc = 0;

    chunkManager = std::make_shared<ChunkManager>();
    auto renderInfo = std::make_shared<ChunkRenderInfo>();

    std::vector<ChunkCoordinate> requestedChunks;

    std::chrono::time_point tp1 = std::chrono::steady_clock::now();
    std::chrono::time_point tp2 = std::chrono::steady_clock::now();

    ChunkCoordinate chunkCoord { 0, 0, 0 };

    AABBOffsets playerAABB {
        glm::vec3(-0.5,  0, -0.5),
        glm::vec3( 0.5,  1,  0.5)
    };

    bool doPhysStep = false;
    float airDrag = 0.1f;     // Less drag in air
    float groundDrag = 4.0f;  // More drag on the ground
    float jumpHeight = 0.18;

    playerData.movementDirection = glm::vec3(0, 0, 0);
    playerData.velocity = glm::vec3(0, 0, 0);
    playerData.acceleration = glm::vec3(0, -0.005, 0);

    // Main render loop
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        auto frameBegin = std::chrono::steady_clock::now();

        double dt = (tp2 - tp1).count() / (double) (1e6 * 14);
        
        int chunkX = floorf(playerData.playerPosition.x / CHUNK_X);
        int chunkY = floorf(playerData.playerPosition.y / CHUNK_Y);
        int chunkZ = floorf(playerData.playerPosition.z / CHUNK_Z);

        chunkCoord = ChunkCoordinate{ chunkX, chunkY, chunkZ };

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

                case PacketType::ChunkData: {
                    ChunkData cr;
                    cr.decodePacket(message);

                    if (chunkManager->getChunks().contains(cr.chunkCoord)) break;

                    auto chunk = std::make_shared<Chunk>();
                    chunk->setBlockData(cr.blockData);

                    chunkManager->getChunks()[cr.chunkCoord] = chunk;

                    for (int x=-1; x<=1; x++) {
                        for (int y=-1; y<=1; y++) {
                            for (int z=-1; z<=1; z++) {
                                ChunkCoordinate refChunkCoord{ cr.chunkCoord.x + x,
                                                    cr.chunkCoord.y + y,
                                                    cr.chunkCoord.z + z };

                                if (chunkManager->getChunks().count(refChunkCoord) > 0) {
                                    chunkManager->getChunks().at(refChunkCoord)->markForRedraw();
                                }
                            }
                        }
                    }

                } break;

                case PacketType::ChunkUpdate: {
                    ChunkUpdate cu;
                    cu.decodePacket(message);

                    if (chunkManager->getChunks().count(cu.chunkCoord) < 1) {
                        std::cout << "Recv'd chunk update before chunk was recv'd\n";
                        break;
                    }

                    auto chunk = chunkManager->getChunks().at(cu.chunkCoord);
                    
                    // Apply updates to chunk
                    for (auto& change : cu.changes) {
                        switch(change.type) {
                            case ChunkUpdate::ChangeType::BREAK: {
                                chunk->setChunkBlock(change.coord, 0);
                            } break;

                            case ChunkUpdate::ChangeType::PLACE: {
                                chunk->setChunkBlock(change.coord, change.blockID);
                            } break;
                        }
                    }

                    if (cu.changes.size() > 0) {
                        chunk->markForRedraw();
                    }
                } break;
            }
        }

        if (networkClient.isConnected()) {
            // Do network stuff...
            if (fc % 5 == 0) {
                ENetPacket* p = playerData.convToPacket();
                networkClient.addToOutQueue(p);
            }
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

            // Movement keys
            glm::vec3 movement = glm::vec3(0, 0, 0);
            auto looking = getLookingVector(playerData.cameraPitch, playerData.cameraYaw);

            if (doPhysStep) {
                glm::vec3 movement(0.0f);

                // Get movement input
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    movement += glm::vec3(looking.x, 0, looking.z);
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    movement -= glm::vec3(looking.x, 0, looking.z);
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    movement += glm::vec3(looking.z, 0, -looking.x);
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    movement += -glm::vec3(looking.z, 0, -looking.x);
                }

                // Normalize movement direction
                glm::vec3 movementDirection = (glm::length(movement) > 0.001f) ? glm::normalize(movement) : glm::vec3(0.0f);

                // Store the movement direction for physics calculations
                playerData.movementDirection = movementDirection;  // <- NEW variable in your struct

                // Update velocity instead of directly modifying position
                playerData.velocity += movementDirection * physSpeed;

                // Apply jump force correctly
                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerData.isGrounded) {
                    playerData.velocity.y += jumpHeight; // Jump applies force, no instant position change
                }
            } else {
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

                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !doPhysStep) {
                    movement -= glm::vec3(0, 1, 0);
                }

                if (glm::length(movement) > 0.001f) {
                    playerData.playerPosition += glm::normalize(movement) * moveSpeed;
                }
            }
        }

        // Physics loop
        if (doPhysStep) {
            playerData.isGrounded = false;

            // Apply acceleration (including gravity)
            playerData.velocity += playerData.acceleration * glm::vec3(dt);

            float dragFactor = playerData.isGrounded ? groundDrag : airDrag;
            playerData.velocity.x *= std::exp(-dragFactor * dt);
            playerData.velocity.z *= std::exp(-dragFactor * dt);

            // Process movement per axis: X → Y → Z
            glm::vec3 movementAxes[] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) };

            for (const auto& axis : movementAxes) {
                glm::vec3 attemptedMovement = axis * playerData.velocity * glm::vec3(dt);
                glm::vec3 originalPosition = playerData.playerPosition;
                playerData.playerPosition += attemptedMovement;

                // Check for collisions along this axis
                auto collisionChunks = getPossibleCollisionChunks(playerData.playerPosition, playerAABB, chunkManager->getChunks());
                std::optional<AABB> coll;

                while ((coll = getAABBCollidingWithChunks(playerData.playerPosition, playerAABB, collisionChunks)).has_value()) {
                    glm::vec3 mtv = resolveAABBCollision(playerData.playerPosition, playerAABB, coll->origin, coll->offsets, attemptedMovement);
                    
                    if (axis.x != 0.0f) {
                        playerData.playerPosition.x = originalPosition.x; // Stop movement
                        playerData.velocity.x = 0.0f;
                    }
                    if (axis.y != 0.0f) {
                        playerData.playerPosition.y = originalPosition.y; // Stop movement
                        playerData.velocity.y = 0.0f;

                        if (mtv.y > 0.0f) {
                            playerData.isGrounded = true;
                        }
                    }
                    if (axis.z != 0.0f) {
                        playerData.playerPosition.z = originalPosition.z; // Stop movement
                        playerData.velocity.z = 0.0f;
                    }
                }
            }
        }

        glm::mat4 view = glm::lookAt(
            playerData.playerPosition + glm::vec3(0, 2, 0), 
            playerData.playerPosition + glm::vec3(0, 2, 0) + getLookingVector(playerData.cameraPitch, playerData.cameraYaw),
            glm::vec3(0, 1, 0)
        );
        glm::mat4 projection = glm::perspective(glm::radians(70.f), aspectRatio, 0.01f, 10000.f);

        glm::mat4 viewProjection = projection * view;

        {
            playerShader->use();
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, playerVertexBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glBindBuffer(GL_ARRAY_BUFFER, playerUVBuffer);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glm::mat4 playerMVP = viewProjection * glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 0.5, 0)), glm::vec3(0.5, 0.5, 0.5));

            glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &playerMVP[0][0]);
            for (auto& player : players) {
                glUniform3fv(positionUniform, 1, &player.second.playerPosition[0]);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }

        chunkManager->renderWorld(renderInfo, viewProjection);
        chunkManager->tick();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Connection window");
            ImGui::Text("Chunk X: %d, Y: %d, Z: %d", chunkX, chunkY, chunkZ);
            ImGui::Text("XYZ: %.2f %.2f %.2f", playerData.playerPosition.x, playerData.playerPosition.y, playerData.playerPosition.z);

            ImGui::InputText("Host IP", hostIP, 20);
            ImGui::InputInt("Host Port", &port);

            if (networkClient.isConnected()) {
                if (ImGui::Button("Disconnect")) {
                    networkClient.disconnect();
                }
            } else {
                if (ImGui::Button("Connect")) {
                    networkClient.connectToHost(std::string(hostIP), port);
                    players.clear();
                }
            }

            ImGui::InputFloat3("Velocity", &playerData.velocity[0]);
            ImGui::InputFloat3("Acceleration", &playerData.acceleration[0]);

            ImGui::InputFloat("Floor Drag", &groundDrag);
            ImGui::InputFloat("Air Drag", &groundDrag);

            ImGui::InputFloat("Jump height", &jumpHeight);
            ImGui::InputFloat("Speed", &physSpeed, 0, 0, "%.6f");

            if (!doPhysStep)  {
                if (ImGui::Button("Enable physics")) doPhysStep = true;
            } else {
                if (ImGui::Button("Disable physics")) doPhysStep = false;
                
            }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glViewport(0, 0, windowWidth, windowHeight);

        auto frameEnd = std::chrono::steady_clock::now();
        std::int64_t diff = (frameEnd - frameBegin).count();

        tp1 = tp2;
        tp2 = frameEnd;

        // If frame took less than 16ms to complete than sleep for rest of time
        if (diff < (1e6 * 14)) {
            std::int64_t sleepTime = (1e6 * 14) - diff;
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
        }

        fc++;
    }

    chunkManager->unloadChunksGracefully();
    
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