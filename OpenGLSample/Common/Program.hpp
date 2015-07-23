//
//  Program.hpp
//  OpenGLSample
//
//  Created by Jacky on 15/7/15.
//  Copyright (c) 2015年 Jacky. All rights reserved.
//

#ifndef OpenGLSample_Program_hpp
#define OpenGLSample_Program_hpp

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Program{
public:
    Program(const char* vertexName, const char* fragmentName){
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexName);
            fShaderFile.open(fragmentName);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar * fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // Print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // Print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Shader Program
        this->_program = glCreateProgram();
        glAttachShader(this->_program, vertex);
        glAttachShader(this->_program, fragment);
        
        // Bind Predefined VertexAttribs
        bindPredefinedVertexAttribs();
        
        //Link program
        glLinkProgram(this->_program);
        // Print linking errors if any
        glGetProgramiv(this->_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->_program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        logVertexAttribsAndUniforms();
    }
    
    void use(){
        glUseProgram(this->_program);
    }
    
    GLuint& getProgram(){
        return _program;
    }
    
    ~Program()
    {
        glDeleteProgram(_program);
    }
    
private:
    GLuint _program;
    //Catch from Cocos2d-x
    void bindPredefinedVertexAttribs()
    {
        static const struct {
            const char *attributeName;
            int location;
        } attribute_locations[] =
        {
            {"position", 0},
            {"normal", 1},
            {"texCoords", 2},
        };
        
        const int size = sizeof(attribute_locations) / sizeof(attribute_locations[0]);
        
        for(int i = 0; i < size; i++) {
            glBindAttribLocation(this->_program, attribute_locations[i].location, attribute_locations[i].attributeName);
        }
    }
    //Catch from Cocos2d-x
    void logVertexAttribsAndUniforms()
    {
        logVertexAttribs();
        logUniforms();
    }
    void logVertexAttribs()
    {
        struct VertexAttrib
        {
            GLuint location;
            GLint size;
            GLenum type;
            std::string name;
            void log(){
                std::cout << "name: " <<  name << ";" << " location: " << location << std::endl;
            }
        };
        GLint activeAttributes;
        GLint length;
        glGetProgramiv(this->_program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
        std::cout << "[attribute] : " << activeAttributes << std::endl;
        if(activeAttributes > 0)
        {
            VertexAttrib attribute;
            glGetProgramiv(this->_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
            if(length > 0)
            {
                GLchar* attribName = (GLchar*) alloca(length + 1);
                for(int i = 0; i < activeAttributes; ++i)
                {
                    glGetActiveAttrib(this->_program, i, length, nullptr, &attribute.size, &attribute.type, attribName);
                    attribName[length] = '\0';
                    attribute.name = std::string(attribName);
                    attribute.location = glGetAttribLocation(this->_program, attribName);
                    attribute.log();
                }
            }
        }

    }
    void logUniforms()
    {
        struct Uniform
        {
            GLint location;
            GLint size;
            GLenum type;
            std::string name;
            void log(){
                std::cout << "name: " <<  name << ";" << " location: " << location << std::endl;
            }
        };
        GLint activeUniforms;
        GLint length;
        glGetProgramiv(this->_program, GL_ACTIVE_UNIFORMS, &activeUniforms);
        std::cout << "[uniform] : " << activeUniforms << std::endl;
        if(activeUniforms > 0)
        {
            Uniform uniform;
            glGetProgramiv(this->_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
            if(length > 0)
            {
                GLchar* uniformName = (GLchar*)alloca(length + 1);
                for(int i = 0; i < activeUniforms; ++i)
                {
                    glGetActiveUniform(this->_program, i, length, nullptr, &uniform.size, &uniform.type, uniformName);
                    uniformName[length] = '\0';
                    uniform.name = std::string(uniformName);
                    uniform.location = glGetUniformLocation(this->_program, uniformName);
                    uniform.log();
                }
            }
        }
    }
};

#endif
