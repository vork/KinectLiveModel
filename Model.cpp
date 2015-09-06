#include "Model.h"
#include <limits.h>


Landscape3DRenderer::Landscape3DRenderer()
{

}

Landscape3DRenderer::Landscape3DRenderer(const Landscape3DRenderer& other)
{
}

Landscape3DRenderer::~Landscape3DRenderer()
{
}

bool Landscape3DRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool result;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void Landscape3DRenderer::Release()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void Landscape3DRenderer::Render(ID3D11DeviceContext* deviceContext, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, LightType* light, CameraType* camera)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext, texShader, world, view, projection, light, camera);

	return;
}

int Landscape3DRenderer::GetIndexCount(int index)
{
	return m_Entries[index].NumIndices;
}

ID3D11ShaderResourceView* Landscape3DRenderer::GetTexture()
{
	return m_Texture.GetTexture();
}

void Landscape3DRenderer::calculateBoundingBox(const aiMesh* pMesh, BoundingBox* bounding_box)
{
	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		auto vertex = pMesh->mVertices[i];
		if (bounding_box->max_x < vertex.x)
		{
			bounding_box->max_x = vertex.x;
		}
		if (bounding_box->min_x > vertex.x)
		{
			bounding_box->min_x = vertex.x;
		}
		if (bounding_box->max_y < vertex.y)
		{
			bounding_box->max_y = vertex.y;
		}
		if (bounding_box->min_y > vertex.y)
		{
			bounding_box->min_y = vertex.y;
		}
		if (bounding_box->max_z < vertex.z)
		{
			bounding_box->max_z = vertex.z;
		}
		if (bounding_box->min_z > vertex.z)
		{
			bounding_box->min_z = vertex.z;
		}

	}
}

void Landscape3DRenderer::InitMesh(unsigned int Index, const aiMesh* pMesh, ID3D11Device* device)
{
	m_Entries[Index].MaterialIndex = pMesh->mMaterialIndex;

	VertexType* Vertices;
	int* Indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of indices in the index array.
	int indexCount = 0;
	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		if (pMesh->mFaces[i].mNumIndices == 3) //only allow triangles
		{
			indexCount = indexCount + 3;
		} else
		{
			printf("Error parsing Faces. Try to Re-Export model from 3d package!");
			return;
		}
	}

	m_Entries[Index].NumIndices = indexCount;

	// Set the number of vertices in the vertex array.
	int vertexCount = pMesh->mNumVertices;

	// Create the vertex array.
	Vertices = new ShaderStructures::VertexType[vertexCount];
	if (!Vertices)
	{
		return;
	}

	// Create the index array.
	Indices = new int[indexCount];
	if (!Indices)
	{
		return;
	}

	BoundingBox box;
	box.min_x = INT_MAX;
	box.max_x = INT_MIN;
	box.min_y = INT_MAX;
	box.max_y = INT_MIN;
	box.min_z = INT_MAX;
	box.max_z = INT_MIN;

	calculateBoundingBox(pMesh, &box);

	int max, min;

	max = max(box.max_x, max(box.max_y, box.max_z));
	min = min(box.min_x, min(box.min_y, box.min_z));

	int range = max - min;

	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		XMFLOAT3 pos(pMesh->mVertices[i].x /*/ range * 5*/, pMesh->mVertices[i].y  /*/ range * 5*/, pMesh->mVertices[i].z  /*/ range * 5*/);
		Vertices[i].position = pos;
		if (pMesh->GetNumUVChannels() > 0) {
			XMFLOAT2 texcoord(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
			Vertices[i].texture = texcoord;
		}
		XMFLOAT3 normals(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
		Vertices[i].normal = normals;
	}

	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		Indices[i * 3 + 0] = pMesh->mFaces[i].mIndices[0];
		Indices[i * 3 + 1] = pMesh->mFaces[i].mIndices[1];
		Indices[i * 3 + 2] = pMesh->mFaces[i].mIndices[2];
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_Entries[Index].VB);
	if (FAILED(result))
	{
		return;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = Indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_Entries[Index].IB);
	if (FAILED(result))
	{
		return;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] Vertices;
	Vertices = 0;

	delete[] Indices;
	Indices = 0;

	return;
}

bool Landscape3DRenderer::InitMaterials(const aiScene* pScene, const std::string& Filename, ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Materials.resize(pScene->mNumMaterials);
	mat_Count = pScene->mNumMaterials;
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		aiColor3D ambient(1.f, 1.f, 1.f);
		//aiColor3D ambient(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		m_Materials[i].materialAmbient = XMFLOAT4(ambient.r, ambient.g, ambient.b, 1.f);

		aiColor4D diffuse(1.f, 1.f, 1.f, 1.f);
		//aiColor4D diffuse(0.f, 0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		m_Materials[i].materialDiffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a); //TODO hopefully the order is correct

		aiColor3D specular(1.f, 1.f, 1.f);
		//aiColor3D specular(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		m_Materials[i].materialSpecular = XMFLOAT4(specular.r, specular.g, specular.b, 1.f);

		aiColor3D emissive(1.f, 1.f, 1.f);
		//aiColor3D emissive(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		m_Materials[i].materialEmissive = XMFLOAT4(emissive.r, emissive.g, emissive.b, 1.f);

		float shininess = 0.f;
		pMaterial->Get(AI_MATKEY_SHININESS, shininess);
		m_Materials[i].materialPower = shininess;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				char* path = Path.data;
				if (!m_Texture.Initialize(device, context, path))
				{
					printf("Error loading texture '%s'\n", Path.data);
					m_Texture.Release();
					return false;
				}
			}
		}
	}

	return true;
}

bool Landscape3DRenderer::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* context)
{
	Assimp::Importer Importer;
	const aiScene* pScene = NULL;
	const aiMesh* pMesh = NULL;
	const aiMaterial* pMat = NULL;

	std::string filename = "Snow Biom C4D Small.dae"; //"World.dae";

	//Possible flags:  aiProcess_TransformUVCoords | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph 
	pScene = Importer.ReadFile(filename.c_str(), 
		aiProcess_Triangulate |
		aiProcess_SplitLargeMeshes |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType |
		aiProcess_PreTransformVertices
	);
	
	if (!pScene)
	{
		printf("Error parsing '%s': '%s'\n", filename.c_str(), Importer.GetErrorString());
		return false;
	}

	m_Entries.resize(pScene->mNumMeshes);
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, device);
	}

	return InitMaterials(pScene, filename, device, context);
}

void Landscape3DRenderer::ShutdownBuffers()
{
	// Release the MeshEntries
	for (int i = 0; i < m_Entries.size(); i++)
	{
		m_Entries[i].IB->Release();
		m_Entries[i].IB = 0;
		m_Entries[i].VB->Release();
		m_Entries[i].IB = 0;
	}

	m_Entries.clear();
}

void Landscape3DRenderer::RenderBuffers(ID3D11DeviceContext* deviceContext, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, LightType* light, CameraType* camera)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		if (m_Entries[i].NumIndices == 0)
		{
			continue;
		}

		MeshEntry curEntry = m_Entries[i];

		//deviceContext->UpdateSubresource(curEntry.VB, 0, NULL, , 0, 0);
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &curEntry.VB, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(curEntry.IB, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		MaterialType curMaterial = m_Materials[curEntry.MaterialIndex];

		ID3D11ShaderResourceView* texture = m_Texture.GetTexture();
		bool result = texShader->Render(deviceContext, curEntry.NumIndices, *world, *view, *projection, texture, &curMaterial, camera, light);
		if (!result)
		{
			return;
		}
	}
}

void Landscape3DRenderer::ReleaseTexture()
{
	m_Texture.Release();
}