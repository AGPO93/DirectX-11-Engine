#include "Model.h"
#include <fstream>
#include <vector>

using namespace std;

struct Vertex
{
	float x, y;
	float r, g, b;
};

Model::Model()
{
	//CreateMesh();
	CreateShaders(m_device);
}

Model::~Model()
{
	//m_vertexBuffer->Release();
	//m_vertexShader->Release();
	//m_pixelShader->Release();
	//m_inputLayout->Release();
}

void Model::CreateMesh()
{
	//Vertex vertices[] = {
	//{ -1, -1, 1, 0, 0 },
	//{ 0, 1, 0, 1, 0 },
	//{ 1, -1, 0, 0, 1 },
	//};

	//create vertex buffer
	//auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	//D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	//vertexData.pSysMem = vertices;

	//m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
}

vector<char> readfile(const char *filename)
{
	vector<char> res;
	FILE * file = NULL;
	fopen_s(&file, filename, "rb");
	while (!feof(file))
	{
		char c;
		fread(&c, sizeof(char), 1, file);
		res.push_back(c);
	}
	fclose(file);
	return res;
}

void Model::CreateShaders(ID3D11Device*	m_pDevice)
{
	/*std::ifstream vsFile("VertexShader.cso", ios::binary);
	std::ifstream psFile("PixelShader.cso", ios::binary);

	vector<char> vsData = { istreambuf_iterator<char>(vsFile), istreambuf_iterator <char>() };
	vector<char> psData = { istreambuf_iterator<char>(psFile), istreambuf_iterator <char>() };*/
	vector<char> vsData = readfile("VertexShader.cso");
	vector<char> psData = readfile("PixelShader.cso");

	m_pDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);
	m_pDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);

	//create input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_pDevice->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);
}

void Model::Draw(ID3D11DeviceContext*	m_pDeviceContext)
{

	//bind shaders
	m_pDeviceContext->IASetInputLayout(m_inputLayout);
	m_pDeviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//bind vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	m_pDeviceContext->Draw(3, 0);
}

