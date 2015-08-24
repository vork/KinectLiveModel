#include "Landscape3DSceneRenderer.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace KinectLiveAnimation;

Landscape3DSceneRenderer::Landscape3DSceneRenderer() {

}

void Landscape3DSceneRenderer::Initialise(ID3D11Device *device) {
	m_pDevice = device;
}

void Landscape3DSceneRenderer::Shutdown() {
	vsBlob->Release();
	psBlob->Release();

	free(m_pConstantBuffer);
	free(m_pVertexBuffer);
	free(m_pIndexBuffer);
	free(m_pInputLayout);
	free(*m_ppVertexShader);
	free(*m_ppPixelShader);
	free(m_pRasterState);
	free(m_pDevice);
	vsBlob->Release();
	psBlob->Release();
	free(m_pVertecies); //TODO check if everything is cleared
}

bool Landscape3DSceneRenderer::LoadMesh(const std::string& Filename)
{
	Assimp::Importer Importer;
	const aiScene *pScene = NULL;
	const aiMesh *pMesh = NULL;

	pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_ValidateDataStructure | aiProcess_FindInvalidData);

	if (!pScene)
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
		return false;
	}

	pMesh = pScene->mMeshes[0];
	if (!pMesh)
	{
		printf("Error Finding Model In file.  Did you export an empty scene?");
		return false;
	}

	for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
	{
		if (pMesh->mFaces[i].mNumIndices == 3) //only allow triangles
		{
			m_NumIndices = m_NumIndices + 3;
		}

		else
		{
			printf("Error parsing Faces. Try to Re-Export model from 3d package!");
			return false;
		}
	}

	m_NumFaces = pMesh->mNumFaces;
	m_NumVertecies = pMesh->mNumVertices;

	m_pVertecies = new Vertex[m_NumVertecies];

	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		Vertex *pvertex = new Vertex{
			D2D1_VECTOR_3F{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
			//D2D1_VECTOR_2F{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }, 
			D2D1_VECTOR_3F{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }
		};
		m_pVertecies[i] = *pvertex;
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = m_pVertecies;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);

	HRESULT hr;

	
	hr = m_pDevice->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&m_pVertexBuffer
	);

	if (FAILED(hr)) {
		return;
	}

	unsigned int* indices = new unsigned int[m_NumIndices];

	for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
		indices[i * 3 + 0] = pMesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = pMesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = pMesh->mFaces[i].mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
	
	hr = m_pDevice->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&m_pIndexBuffer
	);

	if (FAILED(hr)) {
		return;
	}

	//TODO add textures

	// Compile vertex shader shader
	vsBlob = nullptr;
	HRESULT hr = CompileShader(L"SimpleVertexShader.hlsl", "main", "vs_5_0", &vsBlob);
	if (FAILED(hr))
	{
		printf("Failed compiling vertex shader %08X\n", hr);
		return -1;
	}

	// Compile pixel shader shader
	psBlob = nullptr;
	hr = CompileShader(L"SimplePixelShader.hlsl", "main", "ps_5_0", &psBlob);
	if (FAILED(hr))
	{
		vsBlob->Release();
		printf("Failed compiling pixel shader %08X\n", hr);
		return -1;
	}

	printf("Success\n");

	hr = m_pDevice->CreateVertexShader((DWORD*)vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, m_ppVertexShader);

	if (FAILED(hr)) {
		return false;
	}

	hr = m_pDevice->CreatePixelShader((DWORD*)psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, m_ppPixelShader);

	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void Landscape3DSceneRenderer::Render(ID3D11DeviceContext2 *context)
{
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_pInputLayout);

	// Attach our vertex shader.
	context->VSSetShader(*m_ppVertexShader, nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// Attach our pixel shader.
	context->PSSetShader(*m_ppPixelShader, nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_NumIndices, 0, 0);
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}