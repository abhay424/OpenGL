
#ifndef _MODEL_
#define _MODEL_

#include <vector>
#include <map>
#include <string>

struct aiScene;
struct aiNode;
struct aiMesh;

class Mesh;
class Texture;

class Model
{
public:
    Model();

    void Load( const std::string& fileName );
    void Render();
    void Clear();

    ~Model();

private:

    void LoadNode( aiNode* node, const aiScene* scene );
    void LoadMesh( aiMesh* mesh, const aiScene* scene );
    void LoadMaterial( const aiScene* scene );

    // Mesh to index of texture map.
    std::map<Mesh*, unsigned int>   _meshToTexMap;
    std::vector<Texture*>           _textures;
};

#endif // _MODEL_
