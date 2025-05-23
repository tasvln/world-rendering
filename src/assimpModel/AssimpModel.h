#ifndef ASSIMP_MODEL_H
#define ASSIMP_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <string>
#include <vector>

#include <stb/stb_image.h>

#include "../model/model.h"
#include "../mesh/mesh.h"

namespace nsi
{
  class AssimpModel : public Model
  {
  public:
    AssimpModel(const string &filePath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f)) : Model(filePath, position, rotation, scale) {};
    ~AssimpModel() override = default;

    void draw(Shader &shader) override
    {
      for (Mesh &mesh : meshes)
      {
        mesh.draw(shader);
      }
    }

  private:
    std::vector<Mesh> meshes;
    std::map<std::string, Texture> loadedTextures;
    std::string directory;

    // for sRGB monitors
    bool gammaCorrection;

    void loadModel(const std::string &path)
    {
      Assimp::Importer importer;
      const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

      if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
      }

      directory = path.substr(0, path.find_last_of('/'));

      // process ASSIMP's root node recursively
      processNode(scene->mRootNode, scene);
    };

    void processNode(aiNode *node, const aiScene *scene)
    {
      // check parent node
      for (uint i = 0; i < node->mNumMeshes; i++)
      {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
      }

      // process each of the node's children
      for (uint i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
    };

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
      std::vector<Vertex> vertices;
      std::vector<unsigned int> indices;
      std::vector<Texture> textures;

      Vertex vertex;

      for (unsigned int i = 0; i < mesh->mNumVertices; i++)
      {
        vec3 vector;

        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.position = vector;

        if (mesh->HasNormals())
        {
          vector.x = mesh->mNormals[i].x;
          vector.y = mesh->mNormals[i].y;
          vector.z = mesh->mNormals[i].z;
          vertex.normal = vector;
        }

        // texture coordinates
        if (mesh->mTextureCoords[0])
        {
          glm::vec2 vec;

          vec.x = mesh->mTextureCoords[0][i].x;
          vec.y = mesh->mTextureCoords[0][i].y;
          vertex.texCoords = vec;

          // tangent
          vector.x = mesh->mTangents[i].x;
          vector.y = mesh->mTangents[i].y;
          vector.z = mesh->mTangents[i].z;
          vertex.tangent = vector;

          // bitangent
          vector.x = mesh->mBitangents[i].x;
          vector.y = mesh->mBitangents[i].y;
          vector.z = mesh->mBitangents[i].z;
          vertex.biTangent = vector;
        }
        else
        {
          vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
      }

      // get the indices
      for (unsigned int i = 0; i < mesh->mNumFaces; i++)
      {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
          indices.push_back(face.mIndices[j]);
        }
      }

      // process materials
      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

      // 1. diffuse maps
      std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      // 2. specular maps
      std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

      // 3. normal maps
      std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

      // 4. height maps
      std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
      textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

      // return a mesh object created from the extracted mesh data
      return Mesh(vertices, indices, textures);
    };

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string typeName)
    {
      vector<Texture> textures;

      for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
      {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool check = false;

        for (auto &texture : loadedTextures)
        {
          if (texture.path == str.C_Str())
          {
            textures.push_back(texture);
            check = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
            break;
          }
        }

        // check if it's a new texture
        if (!check)
        {
          Texture texture;
          texture.id = textureFromFile(str.C_Str(), this->directory);
          texture.type = typeName;
          texture.path = str.C_Str();
          textures.push_back(texture);
          loadedTextures.push_back(texture);
        }
      }

      return textures;
    }
  };
};

uint textureFromFile(const char *path, const std::string &directory, bool gamma)
{

  std::string filename = string(path);
  filename = directory + '/' + filename;

  unsigned int textureId;
  glGenTextures(1, &textureId);

  int width, height, nrComponents;

  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
    {
      format = GL_RED;
    }
    else if (nrComponents == 3)
    {
      format = GL_RGB;
    }
    else if (nrComponents == 4)
    {
      format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureId;
};

#endif