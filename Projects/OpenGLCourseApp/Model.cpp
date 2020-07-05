
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//!-------------------------------------------------------------------
Model::Model()
{
}

//!-------------------------------------------------------------------
void Model::Load( const std::string& fileName )
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( fileName, aiProcess_Triangulate      |
                                                        aiProcess_FlipUVs          |
                                                        aiProcess_GenSmoothNormals |
                                                        aiProcess_JoinIdenticalVertices );

    if( !scene )
    {
        printf( "Model (%s) failed to load : %s", fileName.c_str(), importer.GetErrorString() );
        return;
    }

    LoadNode(scene->mRootNode, scene);
    LoadMaterial(scene);
}

//!-------------------------------------------------------------------
void Model::LoadNode( aiNode* node, const aiScene* scene )
{
    for( size_t i = 0; i < node->mNumMeshes; i++ )
    {
        LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for( size_t i = 0; i < node->mNumChildren; i++ )
    {
        LoadNode(node->mChildren[i], scene);
    }
}

//!-------------------------------------------------------------------
void Model::LoadMesh( aiMesh* mesh, const aiScene* scene )
{
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    unsigned int nbVertices = 0,
                 nbIndices = 0;
    for( size_t i = 0; i < mesh->mNumVertices; ++i )
    {
        vertices.insert(vertices.end(), { mesh->mVertices[i].x,
                                          mesh->mVertices[i].y,
                                          mesh->mVertices[i].z });

        if( mesh->mTextureCoords[0] )
        {
            vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x,
                                              mesh->mTextureCoords[0][i].y });
        }
        else
        {
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }
        // Will take opposite normal as we dont have it in light direction in shader
        vertices.insert(vertices.end(), { -mesh->mNormals[i].x,
                                          -mesh->mNormals[i].y,
                                          -mesh->mNormals[i].z });
        nbVertices += 8;
    }

    for( size_t i = 0; i < mesh->mNumFaces; ++i )
    {
        aiFace face = mesh->mFaces[i];
        for( size_t j = 0; j < face.mNumIndices; ++j )
        {
            indices.push_back(face.mIndices[j]);
            nbIndices++;
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->Create(&vertices[0], nbVertices, &indices[0], nbIndices, true);
    _meshToTexMap[newMesh] = mesh->mMaterialIndex;
}

//!-------------------------------------------------------------------
void Model::LoadMaterial( const aiScene* scene )
{
    _textures.resize(scene->mNumMaterials);

    for( size_t i = 0; i < scene->mNumMaterials; ++i )
    {
        aiMaterial *material = scene->mMaterials[i];
        _textures[i] = nullptr;

        if( material->GetTextureCount(aiTextureType_DIFFUSE) )
        {
            aiString path;
            if( material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS )
            {
                // Incase texture has absolute path.
                int idx = std::string(path.data).rfind("\\");
                std::string fileName = std::string(path.data).substr(idx + 1);

                std::string texPath = std::string("Textures/") + fileName;

                _textures[i] = new Texture(texPath.c_str());
                if( !_textures[i]->Load(false) )
                {
                    printf("Failed to load texture at: %\n", texPath.c_str());
                    delete _textures[i];
                    _textures[i] = nullptr;
                }
            }
        }
        if( !_textures[i] )
        {
            // If not found texture, will use plane.png texture.
            _textures[i] = new Texture("Textures/plain.png");
            _textures[i]->Load();
        }
    }
}

//!-------------------------------------------------------------------
void Model::Render()
{
    std::map<Mesh*, unsigned int>::iterator it = _meshToTexMap.begin();
    for( ; it != _meshToTexMap.end(); ++it )
    {
        unsigned int materialIndex = it->second;
        if( _textures.size() > materialIndex &&
            _textures[materialIndex] )
        {
            _textures[materialIndex]->Use();
        }
        it->first->Render();
    }
}

//!-------------------------------------------------------------------
void Model::Clear()
{
    std::map<Mesh*, unsigned int>::iterator it = _meshToTexMap.begin();
    while( it != _meshToTexMap.end() )
    {
        if( it->first )
        {
            delete it->first;
        }
        it = _meshToTexMap.erase(it);
    }

    for( size_t i = 0; i < _textures.size(); ++i )
    {
        delete _textures[i];
    }
    _textures.clear();
}

//!-------------------------------------------------------------------
Model::~Model()
{
    Clear();
}
