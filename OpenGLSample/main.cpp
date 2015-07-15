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

//Assimp Includes
#include "Assimp/assimp/Importer.hpp"
#include "Assimp/assimp/scene.h"
#include "Assimp/assimp/postprocess.h"

//Common Includes
#include "Common/Camera.hpp"
#include "Common/Program.hpp"
#include "Common/Mesh.hpp"


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
    
    //Shade operation
    Program objectProgram("object.vs", "object.fs");
    Program lampProgram("lamp.vs", "lamp.fs");
    
    GLuint shaderProgram[2];
    shaderProgram[0] = objectProgram.getProgram();
    shaderProgram[1] = lampProgram.getProgram();
    
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
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_0 = glGetAttribLocation(shaderProgram[0], "position");
    glEnableVertexAttribArray(posAttrib_0);
    glVertexAttribPointer(posAttrib_0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), 0);
    GLint normalAttrib_0 = glGetAttribLocation(shaderProgram[0], "normal");
    glEnableVertexAttribArray(normalAttrib_0);
    glVertexAttribPointer(normalAttrib_0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    GLint texCoordAttrib_0 = glGetAttribLocation(shaderProgram[0], "texCoords");
    glEnableVertexAttribArray(texCoordAttrib_0);
    glVertexAttribPointer(texCoordAttrib_0, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)(6*sizeof(GL_FLOAT)));
    
    //Bind default VAO.
    glBindVertexArray(0);
    
    //Bind second VAO.
    glBindVertexArray(vao[1]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_1 = glGetAttribLocation(shaderProgram[1], "position");
    glEnableVertexAttribArray(posAttrib_1);
    glVertexAttribPointer(posAttrib_1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), 0);
    
    //Bind default VAO.
    glBindVertexArray(0);
    
    //Load textures
    GLuint diffuseMap;
    glGenTextures(1, &diffuseMap);
    int width, height;
    unsigned char* image = SOIL_load_image("container.png", &width, &height, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUseProgram(shaderProgram[0]);
    glUniform1i(glGetUniformLocation(shaderProgram[0], "material.diffuse"), 0);
    
    GLuint specularMap;
    glGenTextures(1, &specularMap);
    image = SOIL_load_image("container_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUseProgram(shaderProgram[0]);
    glUniform1i(glGetUniformLocation(shaderProgram[0], "material.specular"), 1);
    
    //lamp position
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    //GLState
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window)) {
        
        //Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        //Clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Draw Object cube.
        ///Use program, bind vao.
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(vao[0]);
        
        ///Setup mvp and uniform.
        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(shaderProgram[0], "model");
        
        glm::mat4 view = camera.GetViewMatrix();
        GLint uniView =glGetUniformLocation(shaderProgram[0], "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
        GLint uniProjection = glGetUniformLocation(shaderProgram[0], "projection");
        glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        GLint uniViewPos = glGetUniformLocation(shaderProgram[0], "viewPos");
        glUniform3f(uniViewPos, camera.Position.x, camera.Position.y, camera.Position.z);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "dirLight.specular"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[0].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[0].quadratic"), 0.032);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[1].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[1].quadratic"), 0.032);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[2].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[2].quadratic"), 0.032);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[3].linear"), 0.09);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "pointLights[3].quadratic"), 0.032);
        
        glUniform3f(glGetUniformLocation(shaderProgram[0], "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram[0], "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "spotLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "spotLight.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(shaderProgram[0], "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(shaderProgram[0], "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
        
        glUniform1f(glGetUniformLocation(shaderProgram[0], "material.shininess"), 32.0f);
        
        ///Active texture.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        
        ///Draw.
        for (GLuint i=0; i<10; i++) {
            model = glm::mat4();
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        //Draw Lamp Cube.
        ///Use program, bind vao.
        glUseProgram(shaderProgram[1]);
        glBindVertexArray(vao[1]);
        
        ///Setup mvp and uniform.
        uniView =glGetUniformLocation(shaderProgram[1], "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        uniProjection = glGetUniformLocation(shaderProgram[1], "projection");
        glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        for (int i=0; i<4; i++) {
            model = glm::mat4();
            model = glm::scale(glm::translate(model, pointLightPositions[i]), glm::vec3(0.2f));
            uniModel = glGetUniformLocation(shaderProgram[1], "model");
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            
            ///Draw.
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        //Reset.
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        
        //Misc
        glfwSwapBuffers(window);
        glfwPollEvents();
        do_movement();
    }
    
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(2, vao);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
