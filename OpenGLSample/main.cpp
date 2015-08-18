//
//  main.cpp
//  HelloGLFW
//
//  Created by Jacky on 15/5/6.
//  Copyright (c) 2015年 Jacky. All rights reserved.
//

#define GLFW_INCLUDE_GLCOREARB
#include <iostream>

//GLFW Includes
#include "glfw3.h"

//GLM Includes
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

//SOIL Includes
#include "SOIL.h"

//Common Includes
#include "Common/Camera.hpp"
#include "Common/Program.hpp"
#include "Common/Model.hpp"


//Global vars.
const GLuint WIDTH = 800, HEIGHT = 600;
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool firstMouse = true;
bool keys[1024];

//GLFW callbacks.
void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

//Help methods.
void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

//Load cubeMap
GLuint loadCubeMap(vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width, height;
    unsigned char* image;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++) {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return textureID;
}

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
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "HelloWorld", nullptr, nullptr);
    
    if(!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    //GLState
    glEnable(GL_DEPTH_TEST);
    
    Program program("instancing.vs", "instancing.fs");
    
    GLfloat quadVertices[] = {
        // Positions     // Colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
        
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    
    glm::vec2 translations[100];
    int index = 0;
    GLfloat offset = 0.1f;
    for(GLint y = -10; y < 10; y += 2)
    {
        for(GLint x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (GLfloat)x / 10.0f + offset;
            translation.y = (GLfloat)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }
    
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    GLint attribPos = glGetAttribLocation(program.getProgram(), "position");
    glEnableVertexAttribArray(attribPos);
    glVertexAttribPointer(attribPos, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
    GLint attribColor = glGetAttribLocation(program.getProgram(), "color");
    glEnableVertexAttribArray(attribColor);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
    glBindVertexArray(0);
    
    
    while (!glfwWindowShouldClose(window)) {
        
        //Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //Clear
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        program.use();
        for (GLuint i = 0; i < 100; i++) {
            stringstream ss;
            string index;
            ss << i;
            index = ss.str();
            GLint uniOffset = glGetUniformLocation(program.getProgram(), ("offsets[" + index + "]").c_str());
            glUniform2f(uniOffset, translations[i].x, translations[i].y);
        }
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        glBindVertexArray(0);
        
        
        //Misc
        glfwSwapBuffers(window);
        glfwPollEvents();
        do_movement();
    }
    
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
