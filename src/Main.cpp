#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

#define GLEW_STATIC
#include <GL/glew.h>;
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const int window_width = 800, window_height = 800;
static const int texture_width = 100, texture_height = 100; //for the downscaling of the water

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW ERROR: %d: %s\n", error, description);
}
unsigned int createShaderProgram(const std::string& vspath, const std::string& fspath);

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Water effect", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    //SHADERS -----------------------------SHADERS
    //1. Load from a file.
    GLuint program = createShaderProgram("shaders/water.vert", "shaders/water.frag");

    glUseProgram(program);
    glUniform1f(glGetUniformLocation(program, "window_width"), window_width); //time uniform
    //--------------------------------------------

    //VAO STUFF ----------------------------------
    int width = 400, height = 200;
    glUniform1f(glGetUniformLocation(program, "water_height"), height); //time uniform

    std::vector<float> verts; //3 floats per vertex (x, y, wave)
    //The wave float determines if the given vertex should oscillate in the vertex shader.

    //populate with vertex data for water, creates (N-1) partitions.
    int N = 30;
    for (int i = 0; i < N; i++)
    {
        //TOP VERTEX
        //Start at the left edge of the water, then add each partition (width/numOfpartitions) each iteration.)
        verts.push_back( (float)(window_width - width) / 2 + i * ( (float)width / (N - 1) ) ); //x
        verts.push_back( (float)(window_height + height) / 2 ); //y
        verts.push_back(1); //Top vert will always be a wave.

        //BOTTOM VERTEX
        verts.push_back( (float)(window_width - width) / 2 + i * ( (float)width / (N - 1)) ); //x
        verts.push_back((float)(window_height - height) / 2); //y
        verts.push_back(0); //wave will always be 0 on the bottom.
    }
    
    GLuint vao, vbo;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), &verts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::mat4 projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -0.1f, -100.0f);
    //--------------------------------------------

    //FRAMEBUFFER STUFF---------------------------
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint renderTex;
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    //Create an empty image (hence the 0 at the end where data should be.)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //attach the texture to the frame buffer at the attachment point GL_COLOR_ATTACHMENT0 with a LOD of 0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //something failed!
    {
        std::cout << "Error when creating framebuffer!" << std::endl;
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //go back to the default framebuffer.

    //Now, we need a framebuffer to display the texture onto the screen.
    //Cover the entire screen.
    float fbo_quad[] = {
        //pos           //texture
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
    };

    GLuint fbo_vao, fbo_vbo;
    glGenVertexArrays(1, &fbo_vao);
    glBindVertexArray(fbo_vao);
    glGenBuffers(1, &fbo_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, fbo_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_quad), fbo_quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Now set the texture in the appropriate shader
    GLuint screen_program = createShaderProgram("shaders/screen.vert", "shaders/screen.frag");
    glUseProgram(screen_program);
    glUniform1i(glGetUniformLocation(screen_program, "tex"), 0);
    //--------------------------------------------

    //Imgui context stuff
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    bool show_another_window = true;

    glm::mat4 model = glm::mat4(1.0f);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float translate_y = 0.0f, wave_speed = 3.5f;
        static float disturbance = 1.0f, wave_height = 8.78f;
        static bool wireframe = false;
            
        //Thank you ImGui, you're amazing.
        ImGui::Begin("Water editor.");
        ImGui::SliderFloat("Translate_Y", &translate_y, -500, 500);
        ImGui::SliderFloat("wave_speed", &wave_speed, 0, 15.0f);
        ImGui::SliderFloat("wave_height", &wave_height, 0, 50.0f);
        ImGui::SliderFloat("wave_disturbance", &disturbance, 0, 10.0f);
        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, translate_y, 0.0f));
        glm::mat4 mvp = projection*model;

        if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //DRAW TO TEXTURE
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); //Starting from now, render everything into the framebuffer
        glViewport(0, 0, texture_width, texture_height); //Honestly, a little stumped here, mostly found through trial and error. just not sure why it had to be the size of the texture.
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); //clear this framebuffer's color buffer.
        glBindVertexArray(vao);
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]); //mvp uniform
        glUniform1f(glGetUniformLocation(program, "time"), (float)(glfwGetTime() * wave_speed));
        glUniform1f(glGetUniformLocation(program, "disturbance"), disturbance);
        glUniform1f(glGetUniformLocation(program, "wave_height"), wave_height);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, verts.size()/3); //since 3 floats per vertex.

        //DEFAULT FRAMEBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //revert back to the default
        glViewport(0, 0, window_width, window_height);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); //Dont forget to clear the default framebuffer's color buffer.
        glUseProgram(screen_program);
        glBindVertexArray(fbo_vao);
        glBindTexture(GL_TEXTURE_2D, renderTex);
        glDrawArrays(GL_TRIANGLES, 0, 6); //Draw the screen quad.

        // Rendering IMGUI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

unsigned int createShaderProgram(const std::string& vspath, const std::string& fspath)
{
    std::string vscode = "", fscode = "";
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit); //Set up io exceptions
    try
    {
        shaderFile.open(vspath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf(); //Read buffer into the string stream
        shaderFile.close();
        vscode = shaderStream.str();

        shaderFile.open(fspath);
        shaderStream.str(""); //remove old data
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        fscode = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Error reading shader file " << vspath << std::endl;
    }
    const char* c_vscode = vscode.c_str();
    const char* c_fscode = fscode.c_str();

    //2. attach code to shader
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER), fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vshader, 1, &c_vscode, NULL);
    glShaderSource(fshader, 1, &c_fscode, NULL);

    //3. compile and report errors.
    GLchar log[1024];
    GLint success;
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, 1024, NULL, log);
        std::cout << vspath << ": Error compiling Vertex shader: " << log << std::endl;
        return -1;
    }
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, 1024, NULL, log);
        std::cout << fspath << ": Error compiling Fragment shader: " << log << std::endl;
        return -1;
    }

    //4. Attach to a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glValidateProgram(program);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 1024, NULL, log);
        std::cout << "Error linking program: " << log << std::endl;
        return -1;
    }
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    return program;
}