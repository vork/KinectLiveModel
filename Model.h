#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"
#include "TextureShader.h"
#include <vector>
#include <map>

using namespace DirectX;

class Model
{
public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, TextureShader*, XMMATRIX*, XMMATRIX*, XMMATRIX*, XMFLOAT3*, XMFLOAT3*);

	int GetIndexCount(int);

	ID3D11ShaderResourceView* GetTexture(int index);


	int mat_count() const
	{
		return mat_Count;
	}

private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct MaterialType
	{
		XMFLOAT3 materialEmissive;
		XMFLOAT3 materialAmbient;
		XMFLOAT4 materialDiffuse;
		XMFLOAT3 materialSpecular;
		float materialPower;

		XMFLOAT3 dirLightDir;
		XMFLOAT3 dirLightColor;

		string texture;
	};

	struct MeshEntry {
		ID3D11Buffer *VB;
		ID3D11Buffer *IB;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	std::vector<MaterialType> m_Materials;
	std::vector<MeshEntry> m_Entries;
	map<string, Texture> m_Textures;

	int mat_Count;

	bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*, TextureShader*, XMMATRIX*, XMMATRIX*, XMMATRIX*, XMFLOAT3*, XMFLOAT3*);
	void InitMesh(unsigned int Index, const aiMesh* pMesh, ID3D11Device*);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename, ID3D11Device*, ID3D11DeviceContext*);

	void ReleaseTexture();
};

