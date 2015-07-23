//
//  Model.hpp
//  OpenGLSample
//
//  Created by Jacky on 15/7/15.
//  Copyright (c) 2015年 Jacky. All rights reserved.
//

#ifndef OpenGLSample_Model_hpp
#define OpenGLSample_Model_hpp

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include "../GLM/glm.hpp"
#include "../GLM/gtc/matrix_transform.hpp"
#include "SOIL.h"
#include "../Assimp/assimp/Importer.hpp"
#include "../Assimp/assimp/scene.h"
#include "../Assimp/assimp/postprocess.h"

#include "Mesh.hpp"

class Model
{
public:
    Model(const char* filename)
    {
        this->loadModel(filename);
    }
    void draw(GLuint program)
    {
        for (int i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].draw(program);
        }
    }
    ~Model()
    {
        for (int i=0; i<this->textures_loaded.size(); i++) {
            glDeleteTextures(1, &this->textures_loaded[i].id);
        }
        
        for (int i=0; i<this->meshes.size(); i++) {
            glDeleteVertexArrays(1, &this->meshes[i].getVAO());
            glDeleteBuffers(1, &this->meshes[i].getVBO());
            glDeleteBuffers(1, &this->meshes[i].getEBO());
        }
    }
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;

    void loadModel(string filename)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: "<< importer.GetErrorString() << endl;
            return;
        }
        this->directory = filename.substr(0, filename.find_last_of('/'));
        this->processNode(scene->mRootNode, scene);
    }
    void processNode(aiNode* node, const aiScene* scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        for (int i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }
    }
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        
        //vertices
        for (int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }
        
        //indices
        for (int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        //textures
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps_textures = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps_textures.begin(), diffuseMaps_textures.end());
            vector<Texture> specularMaps_textures = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps_textures.begin(), specularMaps_textures.end());        }
        
        return Mesh(vertices, indices, textures);
    }
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
    GLint TextureFromFile(const char* path, string directory)
    {
        //Generate texture ID and load texture data
        string filename = string(path);
        filename = directory + '/' + filename;
        GLuint textureID;
        glGenTextures(1, &textureID);
        int width,height;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        SOIL_free_image_data(image);
        return textureID;
    }
};

#endif
