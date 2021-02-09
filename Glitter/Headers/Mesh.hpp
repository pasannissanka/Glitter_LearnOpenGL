#ifndef MESH_H
#define MESH_H

// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"


#include <string>
#include <vector>


struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<Texture>        textures;
    unsigned int                VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);


    /* ======================TEXTURE NAMING CONVENTION =================================================
    *   Each diffuse texture is named texture_diffuseN, 
        and each specular texture should be named texture_specularN 
        where N is any number ranging from 1 to the maximum number of texture samplers allowed. 

        EX: uniform sampler2D texture_diffuse1;
            uniform sampler2D texture_diffuse2;
            uniform sampler2D texture_diffuse3;
            uniform sampler2D texture_specular1;
            uniform sampler2D texture_specular2;
    
    */
    void Draw(Shader shader);

private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();

};


#endif // !MESH_H
