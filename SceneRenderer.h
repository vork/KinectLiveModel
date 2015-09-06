#pragma once
#include <vector>
#include "ShaderStructures.h"
#include "Texture.h"
#include <map>

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

private:
	std::vector<ShaderStructures::VertexType> Vertices;
	std::vector<ShaderStructures::MaterialType> Materials;
	std::map<std::string, Texture> m_Textures;
	std::vector<int> Indices;

	ID3D11Buffer *VB;
	ID3D11Buffer *IB;
	unsigned int numIndices;
	int mat_Count;
};

