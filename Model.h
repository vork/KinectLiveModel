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
#include "ShaderStructures.h"

using namespace DirectX;
using namespace ShaderStructures;

class Landscape3DRenderer
{
public:
	Landscape3DRenderer();
	Landscape3DRenderer(const Landscape3DRenderer&);
	~Landscape3DRenderer();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	void Release();
	void Render(ID3D11DeviceContext*, TextureShader*, XMMATRIX*, XMMATRIX*, XMMATRIX*, LightType* light, CameraType* camera);

	int GetIndexCount(int);

	ID3D11ShaderResourceView* GetTexture();


	int mat_count() const
	{
		return mat_Count;
	}

private:
	struct MeshEntry {
		ID3D11Buffer *VB;
		ID3D11Buffer *IB;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	struct BoundingBox
	{
		int min_x;
		int max_x;
		int min_y;
		int max_y;
		int min_z;
		int max_z;
	};

	std::vector<MaterialType> m_Materials;
	std::vector<MeshEntry> m_Entries;
	Texture m_Texture;

	int mat_Count;

	bool InitializeBuffers(ID3D11Device*, ID3D11DeviceContext*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*, TextureShader*, XMMATRIX*, XMMATRIX*, XMMATRIX*, LightType* light, CameraType* camera);
	void InitMesh(unsigned int Index, const aiMesh* pMesh, ID3D11Device*);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename, ID3D11Device*, ID3D11DeviceContext*);
	void calculateBoundingBox(const aiMesh* pMesh, BoundingBox* bounding_box);

	void ReleaseTexture();
};

