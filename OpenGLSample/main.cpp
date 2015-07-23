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
    
    //Shade & Model
    Program nanosuitProgram("nanosuit.vs", "nanosuit.fs");
    Program lampProgram("lamp.vs", "lamp.fs");

    Model nanosuitModel("nanosuit/nanosuit.obj");
    
    //Create data.
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    
    //Lamp model data.
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLint posAttrib_1 = glGetAttribLocation(lampProgram.getProgram(), "position");
    glEnableVertexAttribArray(posAttrib_1);
    glVertexAttribPointer(posAttrib_1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), 0);
    
    glBindVertexArray(0);
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3(2.3f, -1.6f, -3.0f),
        glm::vec3(-1.7f, 0.9f, 1.0f)
    };
    
    while (!glfwWindowShouldClose(window)) {
        
        //Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //Clear
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        nanosuitProgram.use();
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(nanosuitProgram.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(nanosuitProgram.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(nanosuitProgram.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        // Direction light
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "dirLight.specular"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        // Point light 1
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].linear"), 0.009);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[0].quadratic"), 0.0032);
        // Point light 2
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].linear"), 0.009);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "pointLights[1].quadratic"), 0.0032);
        // Spot light
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(nanosuitProgram.getProgram(), "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
        
        // Draw the loaded model
        nanosuitModel.draw(nanosuitProgram.getProgram());
        
        //Draw Lamp
        lampProgram.use();
        glBindVertexArray(vao);
        for (int i=0; i<2; i++) {
            glUniformMatrix4fv(glGetUniformLocation(lampProgram.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(lampProgram.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
            model = glm::mat4();
            model = glm::scale(glm::translate(model, pointLightPositions[i]), glm::vec3(0.2));
            glUniformMatrix4fv(glGetUniformLocation(lampProgram.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        //Misc
        glfwSwapBuffers(window);
        glfwPollEvents();
        do_movement();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    
    exit(EXIT_SUCCESS);
}
