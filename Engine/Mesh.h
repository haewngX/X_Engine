#pragma once
#include <vector>
#include <string>
#include "Collision.h"
#include <map>
#include <vector>
#include <GL/glew.h>
#include <Assimp/Importer.hpp>      // C++ importer interface
#include <Assimp/scene.h>       // Output data structure
#include <Assimp/postprocess.h> // Post processing flags

struct Vertex
{
    Vector3 m_pos;
    Vector2 m_tex;
    Vector3 m_normal;

    Vertex() {}

    Vertex(const Vector3& pos, const Vector2& tex, const Vector3& normal)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = normal;
    }
};


class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool Load(const std::string& Filename, class Renderer* renderer);
    void Unload();
    class Texture* GetTexture(size_t index);
    // Get object space bounding box
    const AABB& GetBox() const { return mBox; }
    // Get specular power of mesh
    float GetSpecPower() const { return mSpecPower; }

#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();

        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
            const std::vector<unsigned int>& Indices);

        class VertexArray* mVertexArray;
        unsigned int MaterialIndex;
    };
    std::string mShaderName;
    // Stores object space bounding sphere radius
    float mRadius;
    // Specular power of surface
    float mSpecPower;
    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
    class VertexArray* mVertexArray;
private:
    // AABB collision
    AABB mBox;
    bool InitFromScene(const aiScene* pScene, const std::string& Filename, Renderer* renderer);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename, Renderer* renderer);

    


    
};
