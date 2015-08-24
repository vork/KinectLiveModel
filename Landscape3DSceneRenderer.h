#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>
#include "stdafx.h"

namespace KinectLiveAnimation
{
	class Landscape3DSceneRenderer
	{
	public:
		Landscape3DSceneRenderer();
		void Initialise(ID3D11Device*);
		void Shutdown();
		bool LoadMesh(const std::string& Filename);
		void Render(ID3D11DeviceContext2 *context);

	private:
		struct Vertex {
			D2D1_VECTOR_3F vertex;
			//D2D1_VECTOR_2F texcoord;
			D2D1_VECTOR_3F normal;
		};

		ID3D11InputLayout*		m_pInputLayout;
		ID3D11Buffer*			m_pConstantBuffer;
		ID3D11Buffer*			m_pVertexBuffer;
		ID3D11Buffer*			m_pIndexBuffer;
		ID3D11VertexShader**	m_ppVertexShader;
		ID3D11PixelShader**		m_ppPixelShader;

		ID3D11RasterizerState*	m_pRasterState;
		ID3D11Device*			m_pDevice;

		ID3DBlob*				vsBlob;
		ID3DBlob*				psBlob;

		Vertex*					m_pVertecies;

		int m_NumIndices;
		int m_NumFaces;
		int m_NumVertecies;
	};
}

