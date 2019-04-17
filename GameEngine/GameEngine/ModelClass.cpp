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
	for (int i = 0; i < m_instanceCount; i++)
	{
		ScaleMatrix = XMMatrixScaling(scaleX, scaleY, scaleZ);
		RotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
 		TransformMatrix = XMMatrixTranslation(instances[i].position.x, instances[i].position.y, instances[i].position.z);

		instances[i].instanceMatrix = FudgeMatrix * ScaleMatrix * RotationMatrix * TransformMatrix;
	}
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

void ModelClass::MoveInstance(int i, float newPosX, float newPosY, float newPosZ)
{
	instances[i].position = XMFLOAT3(newPosX, newPosY, newPosZ);
}

XMFLOAT3 ModelClass::GetCurrentPos(int i)
{
	XMFLOAT3 previousPos = instances[i].position;
	return previousPos;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex & instance arrays.
	m_vertexCount = 8;
	m_instanceCount = 104;
	m_indexCount = 36;

	// Create the arrays.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	InstanceType inst;
	instances.assign(m_instanceCount, inst);
	NodeType nods;
	nodes.assign(m_instanceCount, nods);

	for (int i = 0; i < m_instanceCount; i++)
	{
		instances[i].instanceMatrix = XMMatrixIdentity();
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	LoadArrays(vertices, indices);
	DrawGrid();
	AssignNodePositions();

	// Set up the description of the buffers.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

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

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the buffers.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ModelClass::updateInstancesBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	HRESULT result;

	// Set up the description of the instance buffer.

	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances.data();
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

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

void ModelClass::DrawGrid()
{
	float tempPosX = 5;
	float tempPosY = 5;
	int height = 10;
	int width = 10;
	int cubeIndex = 4;

	for (int x = 0; x < height; x++)
	{
		for (int z = 0; z < width; z++)
		{
			instances[cubeIndex].position = XMFLOAT3(x * 3.0f, 0.0f, z * 3.0f);
			tempPosX += 2;
			cubeIndex++;
		}
		tempPosY += 2;
	}
}

void ModelClass::LoadArrays(VertexType* vertices, unsigned long* indices)
{
	// Load the arrays with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	vertices[2].position = XMFLOAT3(1.0f, 1.0f, -1.0f);
	vertices[3].position = XMFLOAT3(1.0f, -1.0f, -1.0f);
	vertices[4].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vertices[5].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	vertices[6].position = XMFLOAT3(1.0f, 1.0f, 1.0f);
	vertices[7].position = XMFLOAT3(1.0f, -1.0f, 1.0f);

	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[4].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[5].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[6].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[7].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// Controllabe cubes positions
	instances[0].position = XMFLOAT3(0.0f, 2.1f, 0.0f);
	instances[1].position = XMFLOAT3(5.5f, 2.1f, 0.0f);
	instances[2].position = XMFLOAT3(5.5f, 2.1f, 0.0f);
	instances[3].position = XMFLOAT3(5.5f, 2.1f, 0.0f);

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
}

void ModelClass::AssignNodePositions()
{
	// Load node array
	for (int i = 4; i < instances.size(); i++)
	{
		nodes[i].position = instances[i].position;
		// Fill the openList with these nodes. (All grid nodes)
		// Starting node will have to be removed and added to
		// closedList later on.
	}
}
