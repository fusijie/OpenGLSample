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
    in float inValue;
    out float geoValue;
    void main() {
        geoValue = sqrt(inValue);
    }
);

const GLchar* geometrySource = GLSL(
    layout(points) in;
    layout(triangle_strip, max_vertices = 3) out;
    in float[] geoValue;
    out float outValue;
    void main() {
        for (int i = 0; i < 3; i++) {
            outValue = geoValue[0] + i;
            EmitVertex();
        }
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
    float data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    //Shade operation
    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkShader(vertexShader);
    GLint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometrySource, nullptr);
    glCompileShader(geometryShader);
    checkShader(geometryShader);
    
    //Program operation
    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    
    const GLchar* feedbackVaryings[]={"outValue"};
    glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    
    glLinkProgram(shaderProgram);
    checkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    //Bind VBO to shader attribute.
    GLint inputAttrib = glGetAttribLocation(shaderProgram, "inValue");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);
    
    //Create transform feedback buffer.
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 3, nullptr, GL_STATIC_READ);
   
    //Perform feedback transform.
    glEnable(GL_RASTERIZER_DISCARD);
    
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
    
    GLuint query;
    glGenQueries(1, &query);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
    
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_POINTS, 0, 5);
    glEndTransformFeedback();
    
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    
    glDisable(GL_RASTERIZER_DISCARD);
    
    glFlush();
    
    //Fetch and print result.
    GLuint primitives;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
    printf("%u primitives written!\n\n", primitives);
    
    GLfloat feedback[15];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
    for (int i = 0; i < 15; i++) {
        printf("%f\n", feedback[i]);
    }
    
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
