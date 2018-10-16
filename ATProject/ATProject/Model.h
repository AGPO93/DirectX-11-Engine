#pragma once
#include <d3d11.h>

class Model
{
public:
	Model();
	~Model();

	void CreateMesh();
	void CreateShaders(ID3D11Device* m_pDevice);
	void Draw(ID3D11DeviceContext* m_pDeviceContext);

private:
	ID3D11Device * m_device = nullptr;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};
