#include "Model.h"


Model::Model()
{

}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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

void Model::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* lightColor, XMFLOAT3* lightDir)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext, texShader, world, view, projection, lightColor, lightDir);

	return;
}

int Model::GetIndexCount(int index)
{
	return m_Entries[index].NumIndices;
}

ID3D11ShaderResourceView* Model::GetTexture(int index)
{
	if (m_Textures.find(m_Materials[index].texture) == m_Textures.end())
	{
		printf("Error finding texture '%p'\n", m_Materials[index].texture.c_str());
	}
	return m_Textures[m_Materials[index].texture].GetTexture();
}

void Model::InitMesh(unsigned int Index, const aiMesh* pMesh, ID3D11Device* device)
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
		}

		else
		{
			printf("Error parsing Faces. Try to Re-Export model from 3d package!");
			return;
		}
	}

	m_Entries[Index].NumIndices = indexCount;

	// Set the number of vertices in the vertex array.
	int vertexCount = pMesh->mNumVertices;

	// Create the vertex array.
	Vertices = new VertexType[vertexCount];
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

	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		XMFLOAT3 pos(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
		Vertices[i].position = pos;
		if (pMesh->GetNumUVChannels() > 0) {
			XMFLOAT2 texcoord(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
			Vertices[i].texture = texcoord;//TODO CHECK THIS!
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
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
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
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
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

bool Model::InitMaterials(const aiScene* pScene, const std::string& Filename, ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Materials.resize(pScene->mNumMaterials);
	mat_Count = pScene->mNumMaterials;
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		aiColor3D emissive(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		m_Materials[i].materialEmissive = XMFLOAT3(emissive.r, emissive.g, emissive.b);

		aiColor3D ambient(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		m_Materials[i].materialAmbient = XMFLOAT3(ambient.r, ambient.g, ambient.b);

		aiColor4D diffuse(0.f, 0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		m_Materials[i].materialDiffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a); //TODO hopefully the order is correct

		aiColor3D specular(0.f, 0.f, 0.f);
		pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		m_Materials[i].materialSpecular = XMFLOAT3(specular.r, specular.g, specular.b);

		float shininess = 0.f;
		pMaterial->Get(AI_MATKEY_SHININESS, shininess);
		m_Materials[i].materialPower = shininess;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				char* path = Path.data;

				if (m_Textures.find(path) == m_Textures.end())
				{
					//not found
					m_Textures[path] = Texture();
					if (!m_Textures[path].Initialize(device, context, path))
					{
						printf("Error loading texture '%s'\n", Path.data);
						m_Textures[path].Shutdown();
						m_Textures.erase(path);
						return false;
					}
				}

				m_Materials[i].texture = path;
			}
		}
	}

	return true;
}

bool Model::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* context)
{
	Assimp::Importer Importer;
	const aiScene* pScene = NULL;
	const aiMesh* pMesh = NULL;
	const aiMaterial* pMat = NULL;

	std::string filename = "Snow Biom C4D Small.dae";

	pScene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_TransformUVCoords | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);
	
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

void Model::ShutdownBuffers()
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

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext, TextureShader* texShader, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, XMFLOAT3* lightColor, XMFLOAT3* lightDir)
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
		InputShaderClass inputShader;
		inputShader.materialEmissive = curMaterial.materialEmissive;
		inputShader.materialAmbient = curMaterial.materialAmbient;
		inputShader.materialDiffuse = curMaterial.materialDiffuse;
		inputShader.materialPower = curMaterial.materialPower;
		inputShader.materialSpecular = curMaterial.materialSpecular;
		inputShader.lightColor = *lightColor;
		inputShader.lightDir = *lightDir;

		if (m_Textures.find(curMaterial.texture) == m_Textures.end())
		{
			printf("Error loading texture '%s'\n", curMaterial.texture.c_str());
			return;
		}

		ID3D11ShaderResourceView* texture = m_Textures[curMaterial.texture].GetTexture();
		bool result = texShader->Render(deviceContext, curEntry.NumIndices, *world, *view, *projection, texture, &inputShader);
		if (!result)
		{
			return;
		}
	}
}

void Model::ReleaseTexture()
{
	std::map<string, Texture>::iterator it;
	for (it = m_Textures.begin(); it != m_Textures.end(); ++it)
	{
		it->second.Shutdown();
	}

	m_Textures.clear();
}