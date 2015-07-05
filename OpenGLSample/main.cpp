//
//  main.cpp
//  HelloGLFW
//
//  Created by Jacky on 15/5/6.
//  Copyright (c) 2015年 Jacky. All rights reserved.
//

#define GLFW_INCLUDE_GLCOREARB
#include <iostream>
#include "glfw3.h"

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void checkShader(GLint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(GL_FALSE == status)
    {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, nullptr, buffer);
        std::string log = buffer;
        printf("%s", log.c_str());
    }
}

void checkProgram(GLint program)
{
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(GL_FALSE == status)
    {
        char buffer[512];
        glGetProgramInfoLog(program, 512, nullptr, buffer);
        std::string log = buffer;
        printf("%s",log.c_str());
    }
}


//Shader source
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec3 position;"
    "in vec3 color;"
    "out vec3 Color;"
    "void main() {"
    "   Color = color;"
    "   gl_Position = vec4(position, 1.0);"
    "}";

const GLchar* fragmentSource =
    "#version 150 core\n"
    "in vec3 Color;"
    "out vec4 finalColor;"
    "void main() {"
    "   finalColor = vec4(Color, 1.0);"
    "}";

const GLchar* fragmentSource_gray =
    "#version 150 core\n"
    "in vec3 Color;"
    "out vec4 finalColor;"
    "void main() {"
    "   float gray = 0.2126 * Color.r + 0.7152 * Color.g + 0.0722 * Color.b;"
    "   finalColor = vec4(gray, gray, gray, 1.0);"
"}";

int main(int argc, const char * argv[]) {
    // insert code here...
    
    GLFWwindow* window;
    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    window = glfwCreateWindow(640, 480, "HelloWorld", nullptr, nullptr);
    
    if(!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glfwSetKeyCallback(window, key_callback);
    
    //Shade operation
    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkShader(vertexShader);
    
    GLint fragmentShader_color = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_color, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader_color);
    checkShader(fragmentShader_color);
    
    GLint fragmentShader_gray = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_gray, 1, &fragmentSource_gray, nullptr);
    glCompileShader(fragmentShader_gray);
    checkShader(fragmentShader_gray);
    
    //Program operation
    GLint shaderProgram[2];
    shaderProgram[0] = glCreateProgram();
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader_color);
    glLinkProgram(shaderProgram[0]);
    checkProgram(shaderProgram[0]);
    
    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader_gray);
    glLinkProgram(shaderProgram[1]);
    checkProgram(shaderProgram[1]);
    
    //Create data.
    float quad_vertices[]={
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Top-left
        0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Top-right
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-left
    };
    
    GLuint quad_element[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    float triangle_vertices[]={
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Left
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Top
    };
    
    //Create a VAO and VBO.
    GLuint vao[2];
    glGenVertexArrays(2, vao);
    GLuint vbo[2];
    glGenBuffers(2, vbo);
    
    //Bind first VAO.
    glBindVertexArray(vao[0]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_element), quad_element, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_0 = glGetAttribLocation(shaderProgram[0], "position");
    glEnableVertexAttribArray(posAttrib_0);
    glVertexAttribPointer(posAttrib_0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), 0);
    GLint colAttrib_0 = glGetAttribLocation(shaderProgram[0], "color");
    glEnableVertexAttribArray(colAttrib_0);
    glVertexAttribPointer(colAttrib_0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    
    //Bind default VAO.
    glBindVertexArray(vao[0]);
    
    //Bind second VAO.
    glBindVertexArray(vao[1]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_1 = glGetAttribLocation(shaderProgram[0], "position");
    glEnableVertexAttribArray(posAttrib_1);
    glVertexAttribPointer(posAttrib_1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), 0);
    GLint colAttrib_1 = glGetAttribLocation(shaderProgram[0], "color");
    glEnableVertexAttribArray(colAttrib_1);
    glVertexAttribPointer(colAttrib_1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    
    //Bind default VAO.
    glBindVertexArray(vao[0]);
    
    while (!glfwWindowShouldClose(window)) {
        
        //Clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw quad.
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(vao[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        glBindVertexArray(0);
        
        //Draw triangle.
        glUseProgram(shaderProgram[1]);
        glBindVertexArray(vao[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteProgram(shaderProgram[0]);
    glDeleteProgram(shaderProgram[1]);
    glDeleteShader(fragmentShader_color);
    glDeleteShader(fragmentShader_gray);
    glDeleteShader(vertexShader);
    glDeleteBuffers(2, vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(2, vao);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
