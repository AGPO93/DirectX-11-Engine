#include "textureclass.h"

TextureClass::TextureClass()
{
	m_texture[0] = 0;
	m_texture[1] = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
	
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{

	// Load the texture in.
	HRESULT result = CreateWICTextureFromFile(device, filename, &m_resource, &m_texture[0]);
	if (FAILED(result))
	{
		return false;
	}
	result = CreateWICTextureFromFile(device, L"../GameEngine/Resources/depositphotos_9798309-stock-photo-seamless-texture-of-wood.jpg", &m_resource, &m_texture[1]);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture[0])
	{
		m_texture[0]->Release();
		m_texture[0] = 0;
	}
	if (m_texture[1])
	{
		m_texture[1]->Release();
		m_texture[1] = 0;
	}

	return;
}

ID3D11ShaderResourceView** TextureClass::GetTexture()
{
	return m_texture;
}