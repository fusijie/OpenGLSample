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

GLuint loadTexture(const char* path, bool alpha = false)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
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
    
    //Shade operation
    Program program("advanced.vs", "advanced.fs");
    
    //Create data.
    GLfloat cubeVertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    GLfloat planeVertices[] = {
        // Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
        5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        
        5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f,  -0.5f, -5.0f,  2.0f, 2.0f
    };
    
    GLfloat transparentVertices[] = {
        // Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    
    //Create a VAO and VBO.
    GLuint vao[3];
    glGenVertexArrays(3, vao);
    GLuint vbo[3];
    glGenBuffers(3, vbo);
    
    //Bind first VAO.
    glBindVertexArray(vao[0]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_0 = glGetAttribLocation(program.getProgram(), "position");
    glEnableVertexAttribArray(posAttrib_0);
    glVertexAttribPointer(posAttrib_0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), 0);
    GLint texCoordAttrib_0 = glGetAttribLocation(program.getProgram(), "texCoords");
    glEnableVertexAttribArray(texCoordAttrib_0);
    glVertexAttribPointer(texCoordAttrib_0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    
    //Bind default VAO.
    glBindVertexArray(0);
    
    //Bind second VAO.
    glBindVertexArray(vao[1]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_1 = glGetAttribLocation(program.getProgram(), "position");
    glEnableVertexAttribArray(posAttrib_1);
    glVertexAttribPointer(posAttrib_1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), 0);
    GLint texCoordAttrib_1 = glGetAttribLocation(program.getProgram(), "texCoords");
    glEnableVertexAttribArray(texCoordAttrib_1);
    glVertexAttribPointer(texCoordAttrib_1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    
    //Bind default VAO.
    glBindVertexArray(0);
    
    //Bind third VAO.
    glBindVertexArray(vao[2]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_2 = glGetAttribLocation(program.getProgram(), "position");
    glEnableVertexAttribArray(posAttrib_2);
    glVertexAttribPointer(posAttrib_2, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), 0);
    GLint texCoordAttrib_2 = glGetAttribLocation(program.getProgram(), "texCoords");
    glEnableVertexAttribArray(texCoordAttrib_2);
    glVertexAttribPointer(texCoordAttrib_2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));

    //Bind default VAO.
    glBindVertexArray(0);
    
    //Load textures
    GLuint cubeTexture = loadTexture("pattern4diffuseblack.jpg");
    GLuint floorTexture = loadTexture("metal.png");
    GLuint grassTexture = loadTexture("grass.png", true);
    
    //Grass position.
    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    vegetation.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    vegetation.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    vegetation.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));
    
    //GLState
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
//    glDepthFunc(GL_ALWAYS);
    
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
        program.use();
        glBindVertexArray(vao[0]);
        
        ///Setup mvp and uniform.
        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(program.getProgram(), "model");
        
        glm::mat4 view = camera.GetViewMatrix();
        GLint uniView =glGetUniformLocation(program.getProgram(), "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
        GLint uniProjection = glGetUniformLocation(program.getProgram(), "projection");
        glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glUniform1d(glGetUniformLocation(program.getProgram(), "texture1"), 0);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw
        glBindVertexArray(vao[1]);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glUniform1d(glGetUniformLocation(program.getProgram(), "texture1"), 0);
        model = glm::mat4();
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Draw
        glBindVertexArray(vao[2]);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1d(glGetUniformLocation(program.getProgram(), "texture1"), 0);
        for (GLuint i = 0 ; i < vegetation.size(); i++) {
            model = glm::mat4();
            model = glm::translate(model, vegetation[i]);
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        //Reset.
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        
        //Misc
        glfwSwapBuffers(window);
        glfwPollEvents();
        do_movement();
    }
    
    glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(3, vao);
    
    glDeleteTextures(1, &floorTexture);
    glDeleteTextures(1, &cubeTexture);
    glDeleteTextures(1, &grassTexture);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
