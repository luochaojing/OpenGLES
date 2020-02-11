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
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // VAO可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。直到VAO解绑
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position
    // index0开始前3个
    
    {
        // 表示是对顶点着色器上哪个in数据的设置。
        // 为了设置 layout (location = 0) in vec3 position;
        // 所以是0
        auto locationInVectexToSet = 0;
        
        // 设置的顶点的类型有几个点
        // layout (location = 0) in vec3 position;
        // 因为是vec3，有三个点(x,y,z)，所以是三
        auto countOfPointPerVectex = 3;
        
        // 顶点里是啥类型
        // 0.5f, 0.0f 这样的，所以是float类型
        auto typeOfInput = GL_FLOAT;
        
        // 是否被标准化，如果为true，那么会被归一化
        auto isNeedNormalize = GL_FALSE;
        
        //步长: 它告诉我们在连续的顶点属性组之间的间隔
        // xyz,rgb,_x_y,
        // xyz,rgb,_x_y,
        // 可以认为第一个xyz和第二个xyz之间的距离，隔着 xyz,rgb,_x_y 8个，所以是8
        auto stride = 8 * sizeof(GLfloat);
        
        // 一组数据xyz,rgb,_x_y是8个，position信息在一组的偏移量，是从0开始的，所以是0
        auto offset = 0;
        
        glVertexAttribPointer(locationInVectexToSet, countOfPointPerVectex, typeOfInput, isNeedNormalize, stride, (GLvoid*)(offset));
        glEnableVertexAttribArray(locationInVectexToSet);
    }
    
    {
        // color: 同理上文
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1); // index = 1 ()
    }

    {
        // texCoord: 同理上文
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }


    // 为啥要解绑？ 因为数据已经上传了，而且后面不会再操作他了？
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    // 图片纹理是为什么不用指定uniformName，就可以关联到片段着色器上的ourTexture上呢❓
    // ✌️ 其实是因为默认会激活这个问题，并且直接上传就能对应上唯一的uniform了？
    // 如果是需要上传多个纹理，代码应该是这样的：
    /*
     // 激活一个纹理，然后绑定，然后通过glUniform1i设置对应名字的uniform，这样就可以设置多个纹理了
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, texture1);
     glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
     
     
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, texture2);
     glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
     */
    
    // 生成图片问题
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char* image = loadImage(&width, &height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
   
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        static float red = 0;
        red++;
        if (red > 100) {
            red = 0;
        }
        
        // Render
        // Clear the colorbuffer
        glClearColor(red / 100.0, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        
        ourShader.Use();
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
    return image;
}
