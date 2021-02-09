/* https://learnopengl.com/ 

*/
// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Camera.hpp"



// Imgui Headers
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdlib>
#include <string>
#include <iostream>

#include "Model.hpp"

// -------------------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void imguiWindow();

// -------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX                 =   mWidth / 2.0f;
float lastY                 =   mHeight / 2.0f;
bool firstMouse             =   true;
bool cameraDisable          =   false;
// timing
float deltaTime             =   0.0f;	// Time between current frame and last frame
float lastFrame             =   0.0f; // Time of last frame
// imgui
bool show_demo_window       =   false;
bool show_another_window    =   false;
ImVec4 clear_color          =   ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//

// -------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // ---------------------------------------------------------------------------------------------------------
    // Load GLFW and Create a Window
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        std::cout << "Failed to Create OpenGL Context" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true); ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // ---------------------------------------------------------------------------------------------------------
    // SHADER INIT
    // build and compile shaders
    // -------------------------
    Shader shader("vertex_shader_lighting.glsl", "fragment_shader_lighting.glsl");
   
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // load models
    // -----------
    Model model_backpack("backpack/backpack.obj");

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Rendering Loop
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    while (glfwWindowShouldClose(mWindow) == false) {
        // per-frame time logic
         // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(mWindow);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        shader.Use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWidth / (float)mHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        model_backpack.Draw(shader);

        // -----------------------------------------------------------------------------------------------------
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        /*
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (cameraDisable)
            ImGui::ShowDemoWindow(&show_demo_window);
        */
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        imguiWindow();
        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(mWindow);
        glfwPollEvents();

    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        cameraDisable = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
        if (cameraDisable)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    // Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// ---------------------------------------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!cameraDisable)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ---------------------------------------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!cameraDisable)
        camera.ProcessMouseScroll(yoffset);
}

// ---------------------------------------------------------------------------------------------------------
void imguiWindow()
{
    static float f = 0.0f;
    static int counter = 0;
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}