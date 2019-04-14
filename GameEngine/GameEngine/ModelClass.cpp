#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
	m_indexBuffer = 0;

	FudgeMatrix = XMMatrixIdentity();
	ModelMatrix = XMMatrixIdentity();

	posX = 0;
	posY = 0;
	posZ = 0;

	roll = 0;
	pitch = 0;
	yaw = 0;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	ModelMatrix = XMMatrixIdentity();

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

void ModelClass::UpdateMatrices()
{
	ScaleMatrix = XMMatrixScaling(scaleX, scaleY, scaleZ);
	RotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	TransformMatrix = XMMatrixTranslation(posX, posY, posZ);

	ModelMatrix = FudgeMatrix * ScaleMatrix * RotationMatrix * TransformMatrix;
}

XMMATRIX ModelClass::GetModelMatrix()
{
	return ModelMatrix;
}

void ModelClass::setRotation(float newPitch, float newYaw, float newRoll)
{
	pitch = newPitch;
	yaw = newYaw;
	roll = newRoll;
}

void ModelClass::setTransform(float newPosX, float newPosY, float newPosZ)
{
	posX = newPosX;
	posY = newPosY;
	posZ = newPosZ;
}

int ModelClass::GetVertexCount()
{
	return m_vertexCount;
}

int ModelClass::GetInstanceCount()
{
	return m_instanceCount;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	InstanceType* instances;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex & instance arrays.
	m_vertexCount = 8;
	m_instanceCount = 4;
	m_indexCount = 36;

	// Create the arrays.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	instances = new InstanceType[m_instanceCount];
	if (!instances)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the arrays with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);  
	vertices[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);  
	vertices[1].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, 1.0f, -1.0f);  
	vertices[2].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f);

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, -1.0f); 
	vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

	vertices[4].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);  
	vertices[4].color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	vertices[5].position = XMFLOAT3(-1.0f, 1.0f, 1.0f); 
	vertices[5].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[6].position = XMFLOAT3(1.0f, 1.0f, 1.0f);
	vertices[6].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f);

	vertices[7].position = XMFLOAT3(1.0f, -1.0f, 1.0f);
	vertices[7].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	instances[0].position = DirectX::XMVectorSet(-1.5f, -1.5f, 5.0f, 1.0f);
	instances[1].position = DirectX::XMVectorSet(-1.5f, 1.5f, 5.0f, 1.0f);
	instances[2].position = DirectX::XMVectorSet(1.5f, -1.5f, 5.0f, 1.0f);
	instances[3].position = DirectX::XMVectorSet(1.5f, 1.5f, 5.0f, 1.0f);

	// Load the index array with data.
	//front face
	indices[0] = 0; 
	indices[1] = 1; 
	indices[2] = 2; 
	indices[3] = 0;  
	indices[4] = 2;
	indices[5] = 3;

	//back face
	indices[6] = 4;
	indices[7] = 6;
	indices[8] = 5;
	indices[9] = 4;
	indices[10] = 7;
	indices[11] = 6;

	//left face
	indices[12] = 4;
	indices[13] = 5;
	indices[14] = 1;
	indices[15] = 4;
	indices[16] = 1;
	indices[17] = 0;

	//right face
	indices[18] = 3;
	indices[19] = 2;
	indices[20] = 6;
	indices[21] = 3;
	indices[22] = 6;
	indices[23] = 7;

	//top face
	indices[24] = 1;
	indices[25] = 5;
	indices[26] = 6;
	indices[27] = 1;
	indices[28] = 6;
	indices[29] = 2;

	//bottom face
	indices[30] = 4;
	indices[31] = 0;
	indices[32] = 3;
	indices[33] = 4;
	indices[34] = 3;
	indices[35] = 7;

	// Set up the description of the buffers.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex & instance data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the buffers.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays
	delete[] vertices;
	vertices = 0;
	delete[] instances;
	instances = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// Release the instance buffer.
	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides & offsets.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set index buffer
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
