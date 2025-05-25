#ifndef MODEL_MESH_H
#define MODEL_MESH_H

#include <glm/glm.hpp>

#include <vector>
#include <string>

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
  glm::vec3 tangent;
  glm::vec3 biTangent;
};

struct Texture
{
  uint id;
  std::string type;
  std::string filePath;
};

namespace nsi
{
  class Mesh
  {
  public:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices, const std::vector<Texture> &textures) : vertices(vertices), indices(indices), textures(textures)
    {
      setupMesh();
    }

    ~Mesh()
    {
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
      glDeleteBuffers(1, &EBO);
    }

    void draw(GLuint shader)
    {
      uint diffuseNr = 1;
      uint specularNr = 1;
      uint normalNr = 1;
      uint heightNr = 1;

      for (uint i = 0; i < textures.size(); i++)
      {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
        {
          number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular")
        {
          number = std::to_string(specularNr++);
        }
        else if (name == "texture_normal")
        {
          number = std::to_string(normalNr++);
        }
        else if (name == "texture_height")
        {
          number = std::to_string(heightNr++);
        }

        std::string uniformName = name + number;
        glUniform1i(glGetUniformLocation(shader, uniformName.c_str()), i);

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
      }

      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      // reset to default
      glActiveTexture(GL_TEXTURE0);
    }

  private:
    uint VAO, VBO, EBO;

    void setupMesh()
    {
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);

      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

      // position attribute
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

      // normal attribute
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));

      // texCoord attribute
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoords)));

      // tangent attribute
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, tangent)));

      // bitangent attribute
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, biTangent)));

      glBindVertexArray(0);
    }
  };
}

#endif