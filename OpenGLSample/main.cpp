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
#define GLSL(src) "#version 150 core\n" #src

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
const GLchar* vertexSource = GLSL(
    in vec2 position;
    in vec3 color;
    out vec3 Color;
    void main() {
        Color = color;
        gl_Position = vec4(position, 1.0, 1.0);
    }
);

const GLchar* fragmentSource = GLSL(
    in vec3 fColor;
    out vec4 finalColor;
    void main() {
        finalColor = vec4(fColor, 1.0);
    }
);

const GLchar* geometrySource = GLSL(
    layout(points) in;
    layout(line_strip, max_vertices = 2) out;
    in vec3 Color[];
    out vec3 fColor;
    void main(){
        fColor = Color[0];
        gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
        EmitVertex();
        EndPrimitive();
    }
);

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
    
    //Create a VAO.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    //Create a VBO, and copy vertices data to it.
    float vertices[] = {
        -0.45f,  0.45f, 1.0f, 0.0f, 0.0f, // Red point
         0.45f,  0.45f, 0.0f, 1.0f, 0.0f, // Green point
         0.45f, -0.45f, 0.0f, 0.0f, 1.0f, // Blue point
        -0.45f, -0.45f, 1.0f, 1.0f, 0.0f, // Yellow point
    };
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Shade operation
    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkShader(vertexShader);
    
    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkShader(fragmentShader);
    
    GLint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometrySource, nullptr);
    glCompileShader(geometryShader);
    checkShader(geometryShader);
    
    //Program operation
    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);
    glLinkProgram(shaderProgram);
    checkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    //Bind VBO to shader attribute.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
    
    
    while (!glfwWindowShouldClose(window)) {
        
        //Clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw
        glDrawArrays(GL_POINTS, 0, 4);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
