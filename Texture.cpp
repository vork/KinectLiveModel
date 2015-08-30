#include "Texture.h"
#include <string>


Texture::Texture()
{
	m_texture = 0;
	m_textureView = 0;
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned char* dataPtr;
	unsigned int i, j, k, rowStart, columnStart;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;


	// Load the targa texture data into memory.
	result = LoadPng(filename, height, width);
	if (!result)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Lock the texture so it can be written to by the CPU.
	hResult = deviceContext->Map(m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hResult))
	{
		return false;
	}

	// Get a pointer to the mapped resource data pointer and cast it into an unsigned char to match the targa data.
	dataPtr = (unsigned char*)mappedResource.pData;

	// Load the targa data into the texture now.
	for (j = 0; j<(unsigned int)height; j++)
	{
		png_bytep row = row_pointers[j];
		// Set the beginning of the row.
		rowStart = j * mappedResource.RowPitch;

		for (i = 0; i<(unsigned int)width; i++)
		{
			png_byte* px = &(row[i * 4]);

			// Set the beginning of the column.
			columnStart = i * 4;

			printf("R: %i G: %i B: %i A: %i", px[0], px[1], px[2], px[3]);
			std::string test = "R: ";
			test += std::to_string(px[0]);
			test += " G: ";
			test += std::to_string(px[1]);
			test += " B: ";
			test += std::to_string(px[2]);
			test += " A: "; 
			test += std::to_string(px[3]);

			// Copy the data in.
			dataPtr[rowStart + columnStart + 0] = px[0]; // Red.
			dataPtr[rowStart + columnStart + 1] = px[1]; // Green.
			dataPtr[rowStart + columnStart + 2] = px[2]; // Blue
			dataPtr[rowStart + columnStart + 3] = px[3]; // Alpha
		}
	}

	// Unlock the texture.
	deviceContext->Unmap(m_texture, 0);

	// Release the targa image data now that it has been loaded into the texture.
	delete[] row_pointers;
	row_pointers = 0;

	// Create the shader-resource view
	srDesc.Format = textureDesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_texture, &srDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa image data.
	if (row_pointers)
	{
		delete[] row_pointers;
		row_pointers = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
}

bool Texture::LoadPng(char* filename, int& height, int& width)
{
	FILE *fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();
	
	png_init_io(png, fp);

	png_read_info(png, info);

	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth = png_get_bit_depth(png, info);

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep)* height);
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
	}

	png_read_image(png, row_pointers);

	fclose(fp);

	return true;
}