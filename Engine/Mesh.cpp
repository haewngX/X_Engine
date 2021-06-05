#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include <fstream>
#include <sstream>
#include "Math.h"
#include "Renderer.h"
#include <rapidjson/document.h>

namespace
{
    union Vertex_GPMESH
    {
        float f;
        uint8_t b[4];
    };
}

Mesh::MeshEntry::MeshEntry()
{
    MaterialIndex = INVALID_MATERIAL;
};

Mesh::MeshEntry::~MeshEntry()
{
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
    const std::vector<unsigned int>& Indices)
{
    // Set the vertex layout/size based on the format in the file
    VertexArray::Layout layout = VertexArray::PosNormTex;
    size_t vertSize = 8;

    mVertexArray = new VertexArray(Vertices.data(), static_cast<unsigned>(Vertices.size()) / vertSize,
        layout, Indices.data(), static_cast<unsigned>(Indices.size()));

}

Mesh::Mesh()
    :mBox(Vector3::Infinity, Vector3::NegInfinity)
    ,mVertexArray(nullptr)
	, mRadius(0.0f)
	, mSpecPower(100.0f)
{
}


Mesh::~Mesh()
{
}


void Mesh::Unload()
{
    //for (unsigned int i = 0; i < m_Entries.size(); i++) {
    //    delete m_Entries[i].mVertexArray;
    //    m_Entries[i].mVertexArray = nullptr;
    //}
    //for (unsigned int i = 0; i < m_Textures.size(); i++) {
    //    delete m_Textures[i];
    //}
}


bool Mesh::Load(const std::string& Filename, Renderer* renderer)
{
    int strlen = Filename.length();
    if (Filename.substr(strlen - 7, strlen - 1) == ".gpmesh")
    {
		std::ifstream file(Filename);
		if (!file.is_open())
		{
			printf("File not found: Mesh %s", Filename.c_str());
			return false;
		}

		std::stringstream fileStream;
		fileStream << file.rdbuf();
		std::string contents = fileStream.str();
		rapidjson::StringStream jsonStr(contents.c_str());
		rapidjson::Document doc;
		doc.ParseStream(jsonStr);

		if (!doc.IsObject())
		{
			printf("Mesh %s is not valid json", Filename.c_str());
			return false;
		}

		int ver = doc["version"].GetInt();

		// Check the version
		if (ver != 1)
		{
			printf("Mesh %s not version 1", Filename.c_str());
			return false;
		}

		mShaderName = doc["shader"].GetString();

		// Set the vertex layout/size based on the format in the file
		VertexArray::Layout layout = VertexArray::PosNormTex;
		size_t vertSize = 8;

		std::string vertexFormat = doc["vertexformat"].GetString();
		if (vertexFormat == "PosNormSkinTex")
		{
			layout = VertexArray::PosNormSkinTex;
			// This is the number of "Vertex" unions, which is 8 + 2 (for skinning)s
			vertSize = 10;
		}

		// Load textures
		const rapidjson::Value& textures = doc["textures"];
		if (!textures.IsArray() || textures.Size() < 1)
		{
			printf("Mesh %s has no textures, there should be at least one", Filename.c_str());
			return false;
		}

		mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

		for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
		{
			// Is this texture already loaded?
			std::string texName = textures[i].GetString();
			Texture* t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// Try loading the texture
				t = renderer->GetTexture(texName);
				if (t == nullptr)
				{
					// If it's still null, just use the default texture
					t = renderer->GetTexture("Assets/Default.png");
				}
			}
			m_Textures.emplace_back(t);
		}

		// Load in the vertices
		const rapidjson::Value& vertsJson = doc["vertices"];
		if (!vertsJson.IsArray() || vertsJson.Size() < 1)
		{
			printf("Mesh %s has no vertices", Filename.c_str());
			return false;
		}

		std::vector<Vertex_GPMESH> vertices;
		vertices.reserve(vertsJson.Size() * vertSize);
		mRadius = 0.0f;
		for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
		{
			// For now, just assume we have 8 elements
			const rapidjson::Value& vert = vertsJson[i];
			if (!vert.IsArray())
			{
				printf("Unexpected vertex format for %s", Filename.c_str());
				return false;
			}

			Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
			mRadius = Math::Max(mRadius, pos.LengthSq());
			mBox.UpdateMinMax(pos);

			if (layout == VertexArray::PosNormTex)
			{
				Vertex_GPMESH v;
				// Add the floats
				for (rapidjson::SizeType j = 0; j < vert.Size(); j++)
				{
					v.f = static_cast<float>(vert[j].GetDouble());
					vertices.emplace_back(v);
				}
			}
			else
			{
				Vertex_GPMESH v;
				// Add pos/normal
				for (rapidjson::SizeType j = 0; j < 6; j++)
				{
					v.f = static_cast<float>(vert[j].GetDouble());
					vertices.emplace_back(v);
				}

				// Add skin information
				for (rapidjson::SizeType j = 6; j < 14; j += 4)
				{
					v.b[0] = vert[j].GetUint();
					v.b[1] = vert[j + 1].GetUint();
					v.b[2] = vert[j + 2].GetUint();
					v.b[3] = vert[j + 3].GetUint();
					vertices.emplace_back(v);
				}

				// Add tex coords
				for (rapidjson::SizeType j = 14; j < vert.Size(); j++)
				{
					v.f = vert[j].GetDouble();
					vertices.emplace_back(v);
				}
			}
		}

		// We were computing length squared earlier
		mRadius = Math::Sqrt(mRadius);

		// Load in the indices
		const rapidjson::Value& indJson = doc["indices"];
		if (!indJson.IsArray() || indJson.Size() < 1)
		{
			printf("Mesh %s has no indices", Filename.c_str());
			return false;
		}

		std::vector<unsigned int> indices;
		indices.reserve(indJson.Size() * 3);
		for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
		{
			const rapidjson::Value& ind = indJson[i];
			if (!ind.IsArray() || ind.Size() != 3)
			{
				printf("Invalid indices for %s", Filename.c_str());
				return false;
			}

			indices.emplace_back(ind[0].GetUint());
			indices.emplace_back(ind[1].GetUint());
			indices.emplace_back(ind[2].GetUint());
		}

		// Now create a vertex array
		mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
			layout, indices.data(), static_cast<unsigned>(indices.size()));
		return true;
    }
    else
    {
        bool Ret = false;
        Assimp::Importer Importer;

        const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

        if (pScene) {
            Ret = InitFromScene(pScene, Filename, renderer);
        }
        else {
            printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
        }

        return Ret;
    }
    
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename, Renderer* renderer)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename, renderer);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(Vector3(pPos->x, pPos->y, pPos->z),
            Vector2(pTexCoord->x, pTexCoord->y),
            Vector3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        //assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename, Renderer* renderer)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = renderer->GetTexture(FullPath);     
            }
        }

        // Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = renderer->GetTexture("Assets/Default.png");
        }
    }

    return Ret;
}

Texture* Mesh::GetTexture(size_t index)
{
    if (index < m_Textures.size())
    {
        return m_Textures[index];
    }
    else
    {
        return nullptr;
    }
}