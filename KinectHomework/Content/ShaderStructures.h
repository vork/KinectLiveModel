#pragma once

namespace KinectHomework
{
    __declspec(align(16)) //16 byte aligned for performance improvements

	// Constant buffer to pass model view projection matrices to the shader. Additional a color information for the pixel shader.
	struct MatrixBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
        DirectX::XMFLOAT3 color;
	};

	// Send vertex data to the shader
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
}