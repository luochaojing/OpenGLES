//
//  main.cpp
//  OpenGLES3
//
//  Created by luochaojing on 2020/2/2.
//  Copyright © 2020 lemon. All rights reserved.
//

#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "SOIL.h"

#include "Shader1.h"

using namespace std;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLFWwindow *createWindow();

unsigned char* loadImage(int *wid, int *height);



// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.7f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


// The MAIN function, from here we start the application and run the game loop
int main() {

    // 创建窗口
    GLFWwindow* window = createWindow();
    
    Shader ourShader("/Users/luochaojing/OpenGLES3/OpenGLES3/ShaderSource/shader.vs",
                     "/Users/luochaojing/OpenGLES3/OpenGLES3/ShaderSource/shader.frag");
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Positions         // Colors
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
       -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top
    };


    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO); // VAO: vertex array obj
    glGenBuffers(1, &VBO);      // VBO: vertex buffer obj
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // VAO可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。直到VAO解绑
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(0));
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 为啥要解绑？ 因为数据已经上传了，而且后面不会再操作他了？
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        
        // Draw our first triangle
//        glUseProgram(shaderProgram);
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        
        /*
         ### ✌️ 所以这里是指定了1的方式，连续用6个点来绘制2个三角形。element是指定对应的index
         1.GL_TRIANGLES：每三个顶之间绘制三角形，之间不连接；
         2.GL_TRIANGLE_FAN：以V0V1V2,V0V2V3,V0V3V4，……的形式绘制三角形；
         3.GL_TRIANGLE_STRIP：顺序在每三个顶点之间均绘制三角形。这个方法可以保证从相同的方向上所有三角形均被绘制。以V0V1V2,V1V2V3,V2V3V4……的形式绘制三角形；
         */
        // 使用elment方法，画独立的三角形，使用6个点.
        // 12: 因为要画4个三角形，所以是12个顶点。
//        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow *createWindow() {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    if (!glfwInit()) {
        std::cout << "glfwInit fialed" << endl;
        return nullptr;
    }
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
#if __APPLE__
    // 在mac上做了兼容:https://github.com/ocornut/imgui/pull/229/commits/6ee65fc5afb6a0f14c88ec3d693d5fc8a2fcfb52
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    
    if (window == nullptr) {
        const char *error;
        glfwGetError(&error);
        std::cout << "Failed to create GLFW window:" << error << std::endl;
        glfwTerminate();
        return nullptr;
    }
    
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }
    

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    return window;
}

// MARK: - load image

unsigned char* loadImage(int *wid, int *height) {
    int _w, _h;
    unsigned char *image = SOIL_load_image("/Users/luochaojing/OpenGLES3/OpenGLES3/wall.jpg", &_w, &_h, 0, SOIL_LOAD_RGB);
    *wid = _w;
    *height = _h;
    /*
     // 生成图片问题
     GLuint texture;
     int imageWidth, imageHeight;
     glGenTextures(1, &texture);
     glBindTexture(GL_TEXTURE_2D, texture);
     unsigned char *image = loadImage(&imageWidth, &imageHeight);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
     SOIL_free_image_data(image);
     glBindTexture(GL_TEXTURE_2D, 0);
     */
    
    return image;
}
