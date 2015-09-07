#include "pch.h"
#include "Grid.h"
#include "Content\ShaderStructures.h"

using namespace KinectHomework;

Grid::Grid()
{


}

void Grid::Initialise(ID3D11Device *device)
{
	int gridWidth = 20;
	float cellWidth = 5.0f;
	float cellHeight = 5.0f; //Squared Grids

	VertexPositionColor *vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	int num = (gridWidth + 1) / 2;
	
	int numInRow = (num * 2 + 1);

	// Calculate the number of vertices in the grid
	m_vertexCount = numInRow * numInRow;

	// Set the index count
	m_indexCount = numInRow * 4;

	vertices = new VertexPositionColor[m_vertexCount];
	indices = new unsigned long[m_indexCount];
	index = 0;

	//Generate the vertices and indices
	for (int i = -num; i <= num; i++)
	{
		for (int j = -num; j <= num; j++)
		{
			vertices[index].pos = XMFLOAT3(cellWidth * i, 0.0f, cellHeight * j);
			vertices[index].color = XMFLOAT3(0.4f, 0.4f, 0.4f);
			index++;
		}
	}

	index = 0;

	// Split the directions.
	// First generate grid lines in one direction and then move to the other
	for (int k = 0; k < numInRow; k++)
	{
		indices[index] = k;
		index++;
		indices[index] = k + (numInRow * (numInRow - 1));
		index++;
	}

	for (int k = 0; k < numInRow * numInRow; k+=numInRow)
	{
		indices[index] = k;
		index++;
		indices[index] = k + numInRow - 1;
		index++;
	}

	// Setup descriptions for the vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColor) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// And create the buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return;
	}

	// Setup descriptions for the index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return;
	}

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE; //Culling isn't really needed for a grid. Duh.
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = false;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	device->CreateRasterizerState(&rasterDesc, &m_rasterState);

	// Release the arrays now that the buffers have been created and loaded.
	delete [] vertices;
	delete [] indices;
}

void Grid::ShutdownBuffers()
{
	//Cleanup
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void Grid::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset
	stride = sizeof(VertexPositionColor);
	offset = 0;

	// activate vertex buffer
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// activate index buffer
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// A Grid is based on Lines. So set the topology to primitive lines.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// and render the grid
	deviceContext->RSSetState(m_rasterState);
	return;
}

