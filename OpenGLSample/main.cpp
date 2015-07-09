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

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

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
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"
    "void main() {"
    "   gl_Position = projection * view * model * vec4(position, 1.0);"
    "}";

const GLchar* fragmentSource_light =
    "#version 150 core\n"
    "out vec4 finalColor;"
    "uniform vec3 objectColor;"
    "uniform vec3 lightColor;"
    "void main() {"
    "   finalColor = vec4(lightColor * objectColor, 1.0);"
    "}";

const GLchar* fragmentSource_lamp =
    "#version 150 core\n"
    "out vec4 finalColor;"
    "void main() {"
    "   finalColor = vec4(1.0);"
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
    
    window = glfwCreateWindow(800, 600, "HelloWorld", nullptr, nullptr);
    
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
    
    GLint fragmentShader_light = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_light, 1, &fragmentSource_light, nullptr);
    glCompileShader(fragmentShader_light);
    checkShader(fragmentShader_light);
    
    GLint fragmentShader_lamp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_lamp, 1, &fragmentSource_lamp, nullptr);
    glCompileShader(fragmentShader_lamp);
    checkShader(fragmentShader_lamp);
    
    //Program operation
    GLint shaderProgram[2];
    shaderProgram[0] = glCreateProgram();
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader_light);
    glLinkProgram(shaderProgram[0]);
    checkProgram(shaderProgram[0]);
    
    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader_lamp);
    glLinkProgram(shaderProgram[1]);
    checkProgram(shaderProgram[1]);
    
    //Create data.
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
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
    glVertexAttribPointer(posAttrib_0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 0);
    
    //Bind default VAO.
    glBindVertexArray(vao[0]);
    
    //Bind second VAO.
    glBindVertexArray(vao[1]);
    
    //Create a VBO, and copy vertices data to it.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Bind VBO to shader attribute.
    GLint posAttrib_1 = glGetAttribLocation(shaderProgram[0], "position");
    glEnableVertexAttribArray(posAttrib_1);
    glVertexAttribPointer(posAttrib_1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), 0);
    
    //Bind default VAO.
    glBindVertexArray(vao[0]);
    
    //lamp position
    glm::vec3 lampPos(1.2f, 1.0f, 2.0f);
    
    while (!glfwWindowShouldClose(window)) {
        
        //Clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw Object cube.
        ///Use program, bind vao.
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(vao[0]);
        
        ///Setup mvp and uniform.
        glm::mat4 model;
        GLint uniModel = glGetUniformLocation(shaderProgram[0], "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        
        glm::mat4 view = glm::lookAt(glm::vec3(3.0f,1.0f,5.0f), glm::vec3(0.0f,0.0f,2.0f), glm::vec3(0.0f,1.0f,0.0f));
        GLint uniView =glGetUniformLocation(shaderProgram[0], "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 projection = glm::perspective(45.0f, 800.0f/600.0f, 0.1f, 100.0f);
        GLint uniProjection = glGetUniformLocation(shaderProgram[0], "projection");
        glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        GLint uniObjectColor = glGetUniformLocation(shaderProgram[0], "objectColor");
        glUniform3f(uniObjectColor, 1.0f, 0.5f, 0.31f);
        
        GLint uniLightColor = glGetUniformLocation(shaderProgram[0], "lightColor");
        glUniform3f(uniLightColor, 1.0f, 0.5f, 1.0f);
        
        ///Draw.
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //Draw Lamp Cube.
        ///Use program, bind vao.
        glUseProgram(shaderProgram[1]);
        glBindVertexArray(vao[1]);
        
        ///Setup mvp and uniform.
        model = glm::mat4();
        model = glm::scale(glm::translate(model, lampPos), glm::vec3(0.2f));
        uniModel = glGetUniformLocation(shaderProgram[1], "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

        uniView =glGetUniformLocation(shaderProgram[1], "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        uniProjection = glGetUniformLocation(shaderProgram[1], "projection");
        glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(projection));
        
        ///Draw.
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //Reset vao.
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteProgram(shaderProgram[0]);
    glDeleteProgram(shaderProgram[1]);
    glDeleteShader(fragmentShader_light);
    glDeleteShader(fragmentShader_lamp);
    glDeleteShader(vertexShader);
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(2, vao);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
