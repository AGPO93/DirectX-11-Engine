#include "ModelClass.h"

#include "OBJImporter.h"
#include <vector>

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;

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

	startNode = new NodeType();
	endNode = new NodeType();
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{
	delete startNode;
	startNode = nullptr;

	delete endNode;
	endNode = nullptr;
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	if (_vertices.size() == 0)
		OBJImporter::ImportMesh(&_vertices, &_indices, "Cube");
	
	ModelMatrix = XMMatrixIdentity();

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
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
	for (int i = 0; i < instances.size(); i++)
	{
		ScaleMatrix = XMMatrixScaling(scaleX, scaleY, scaleZ);
		RotationMatrix = XMMatrixRotationRollPitchYaw(instances[i].rotation.x, instances[i].rotation.y, instances[i].rotation.z);
		//RotationMatrix = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
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

ID3D11ShaderResourceView** ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex & instance arrays.
	m_vertexCount = _vertices.size();
	m_instanceCount = 104;
	m_indexCount = _indices.size();

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
		instances[i].textureID = 1;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	LoadArrays(vertices, indices);
	DrawGrid();

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

void ModelClass::LoadArrays(VertexType* vertices, unsigned long* indices)
{
	// Load the arrays with data.
	for (int i = 0; i < _vertices.size(); ++i)
	{
		vertices[i].position = _vertices[i].position;
		vertices[i].texture = _vertices[i].texture;
		vertices[i].normal = _vertices[i].normal;

		indices[i] = _indices[i];
	}

	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	//vertices[1].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	//vertices[2].position = XMFLOAT3(1.0f, 1.0f, -1.0f);
	//vertices[3].position = XMFLOAT3(1.0f, -1.0f, -1.0f);
	//vertices[4].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	//vertices[5].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	//vertices[6].position = XMFLOAT3(1.0f, 1.0f, 1.0f);
	//vertices[7].position = XMFLOAT3(1.0f, -1.0f, 1.0f);
	//
	//vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	//vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[3].texture = XMFLOAT2(1.0f, 0.0f);
	//vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	//vertices[5].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[6].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[7].texture = XMFLOAT2(1.0f, 0.0f);
	
	// Controllabe cubes positions
	instances[100].position = XMFLOAT3(3.0f, 2.1f, -3.0f);
	instances[100].textureID = 1;
	instances[101].position = XMFLOAT3(3.0f, 2.1f, -6.0f);
	instances[101].textureID = 1;
	instances[102].position = XMFLOAT3(3.0f, 2.1f, -9.0f);
	instances[102].textureID = 1;
	instances[103].position = XMFLOAT3(3.0f, 2.1f, -12.0f);
	instances[103].textureID = 1;

	// Load the index array with data.
	//front face
	//indices[0] = 0;
	//indices[1] = 1;
	//indices[2] = 2;
	//indices[3] = 0;
	//indices[4] = 2;
	//indices[5] = 3;
	//
	////back face
	//indices[6] = 4;
	//indices[7] = 6;
	//indices[8] = 5;
	//indices[9] = 4;
	//indices[10] = 7;
	//indices[11] = 6;
	//
	////left face
	//indices[12] = 4;
	//indices[13] = 5;
	//indices[14] = 1;
	//indices[15] = 4;
	//indices[16] = 1;
	//indices[17] = 0;
	//
	////right face
	//indices[18] = 3;
	//indices[19] = 2;
	//indices[20] = 6;
	//indices[21] = 3;
	//indices[22] = 6;
	//indices[23] = 7;
	//
	////top face
	//indices[24] = 1;
	//indices[25] = 5;
	//indices[26] = 6;
	//indices[27] = 1;
	//indices[28] = 6;
	//indices[29] = 2;
	//
	////bottom face
	//indices[30] = 4;
	//indices[31] = 0;
	//indices[32] = 3;
	//indices[33] = 4;
	//indices[34] = 3;
	//indices[35] = 7;
}

void ModelClass::DrawGrid()
{
	float tempPosX = 5;
	float tempPosY = 5;
	int height = 10;
	int width = 10;
	int cubeIndex = 0;

	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < height; z++)
		{
			// Position the cubes in a grid.
			instances[cubeIndex].position = XMFLOAT3(x * 3.0f, 0.0f, z * 3.0f);
			instances[cubeIndex].rotation = XMFLOAT3(90, 0, 90);
			instances[cubeIndex].textureID = 1;
			tempPosX += 2;
			cubeIndex++;
		}
		tempPosY += 2;
	}

	AssignNodePositions();
	AssignNodeNeighbours(width, height);
}

void ModelClass::AssignNodePositions()
{
	// Init node array
	for (int i = 0; i < (instances.size() - 4); i++)
	{
		nodes[i].position = instances[i].position;
		nodes[i].obstacle = false;
		nodes[i].parent = nullptr;
		nodes[i].visited = false;
		nodes[i].gCost = INFINITY;
		nodes[i].hCost = INFINITY;
	}

	for (int i = 100; i < instances.size(); i++)
	{
		nodes[i].position = instances[0].position;
		nodes[i].obstacle = false;
		nodes[i].parent = nullptr;
		nodes[i].visited = false;
		nodes[i].gCost = INFINITY;
		nodes[i].hCost = INFINITY;
	}
}

void ModelClass::AssignNodeNeighbours(int width, int height)
{
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < height; z++)
		{
			if (z > 0)
				nodes[(z * width + x)].neighbours.push_back(&nodes[(z - 1) * width + (x + 0)]);
			if (z < height - 1)
				nodes[(z * width + x)].neighbours.push_back(&nodes[(z + 1) * width + (x + 0)]);
			if (x > 0)
				nodes[(z * width + x)].neighbours.push_back(&nodes[(z + 0) * width + (x - 1)]);
			if (x < width - 1)
				nodes[(z * width + x)].neighbours.push_back(&nodes[(z + 0) * width + (x + 1)]);
		}
	}
}

void ModelClass::Pathfinding()
{
	startNode->position = nodes[0].position;
	startNode->neighbours = nodes[0].neighbours;
	endNode->position = nodes[goalNode].position;

	auto distance = [](NodeType* a, NodeType* b)
	{
		return sqrtf((a->position.x - b->position.x) * (a->position.x - b->position.x) + (a->position.z - b->position.z) * (a->position.z - b->position.z));
	}; 

	auto heuristic = [distance](NodeType* a, NodeType* b)
	{
		return distance(a, b);
	};

	// Setup
	NodeType* currentNode = startNode;
	startNode->gCost = 0.0f;
	startNode->hCost = heuristic(startNode, endNode);

	list<NodeType*> openList;
	openList.push_back(startNode);

	while (!openList.empty())
	{ // Sort the list in ascending order based on h cost (distance to goal).
		openList.sort([](const NodeType* nodeA, const NodeType* nodeB) {return nodeA->hCost < nodeB->hCost; });

		// Remove visited nodes.
		while (!openList.empty() && openList.front()->visited)
			openList.pop_front();

		// End search when there's no more nodes to test.
		if (openList.empty())
			break;

		// This is to avoid visiting same node more than once.
		currentNode = openList.front();
		currentNode->visited = true;
		
		// Once the destination has been reached
		// Trace back the visited nodes to build the path.
		if (currentNode->position.x == endNode->position.x && currentNode->position.z == endNode->position.z)
		{
			while (currentNode->parent != nullptr)
			{
				path.push_back(currentNode);
				currentNode = currentNode->parent;
			}
			// Should cleanup openList
			createRealPath();
			return;
		}

		// Loop through the node's neighbours
		for (auto nodeNeighbour : currentNode->neighbours)
		{
			// Add it to the list if it hasn't been visited
			// and it's not an obstacle.
			if (!nodeNeighbour->visited && nodeNeighbour->obstacle == 0)
				openList.push_back(nodeNeighbour);

			// Calculate the neighbours potential lowest parent distance.
			float lowestParentDistance = currentNode->gCost + distance(currentNode, nodeNeighbour);

			// Update the gCost if this path is chosen.
			if (lowestParentDistance < nodeNeighbour->gCost)
			{
				nodeNeighbour->parent = currentNode;
				nodeNeighbour->gCost = lowestParentDistance;

				// Update hCost based on distance to the goal position (endNode).
				nodeNeighbour->hCost = nodeNeighbour->gCost + heuristic(nodeNeighbour, endNode);
			}
		}
	}
}

void ModelClass::createRealPath()
{
	for (int i = 0; i < path.size(); i++)
	{
		realPath.push_back(NodeType());

		realPath[i] = *path[i];
	}

	reverse(realPath.begin(), realPath.end());
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
