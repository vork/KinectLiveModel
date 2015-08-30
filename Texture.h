#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <png.h>

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;

	bool LoadPng(char*, int&, int&);

	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
};

