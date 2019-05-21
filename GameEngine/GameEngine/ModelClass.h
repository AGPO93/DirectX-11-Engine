#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

// INCLUDES 
#include <vector>
#include <algorithm>
#include <d3d11.h>
#include <directxmath.h>
#include <list>
#include "TextureClass.h"
using namespace DirectX;
using namespace std;

class ModelClass
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct InstanceType
	{
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMMATRIX instanceMatrix;
		int textureID = 0;
		int goalNodeIndex = 0;
		bool reachedGoal = false;
	};

	struct NodeType
	{
		XMFLOAT3 position;
		bool obstacle = false;
		bool visited = false;
		float gCost = 0; //fLocalGoal
		float hCost = 0; //fGlobalGoal
		vector<NodeType*> neighbours; // might have to initialise for movable cubes.
		NodeType* parent;

		// if node occupied, send to closed list
		// if node obstacle, send to closed list
		// G cost = distance from starting node
		// H cost = distance from goal node
		// F cost = G cost + H cost
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	bool updateInstancesBuffer(ID3D11Device* );
	int GetVertexCount() { return m_vertexCount; }
	int GetInstanceCount() { return m_instanceCount; }
	int GetIndexCount() { return m_indexCount; }
	void UpdateMatrices();
	void setRotation(float newPitch, float newYaw, float newRoll);
	void setTransform(float newPosX, float newPosY, float newPosZ);
	void MoveInstance(int i, float newPosX, float newPosY, float newPosZ);
	void Pathfinding();
	XMMATRIX GetModelMatrix() {return  ModelMatrix;}
	XMFLOAT3 GetCurrentPos(int i);
	ID3D11ShaderResourceView** GetTexture();

	vector<InstanceType> instances;
	vector<NodeType> nodes;
	vector<NodeType*> path;
	vector<NodeType> realPath;

	NodeType* startNode = nullptr;
	NodeType* endNode = nullptr;

	int goalNode = 55;

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	void DrawGrid();
	void LoadArrays(VertexType* vertices, unsigned long* indices);
	void AssignNodePositions();
	void AssignNodeNeighbours(int width, int height);
	void createRealPath();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	ID3D11Buffer *m_vertexBuffer, *m_constantBuffer, *m_instanceBuffer,
				 *m_indexBuffer;

	XMMATRIX TransformMatrix, ScaleMatrix, RotationMatrix, FudgeMatrix, ModelMatrix;


	int m_vertexCount, m_instanceCount, m_indexCount;

	std::vector<ModelClass::VertexType> _vertices;
	std::vector<unsigned long> _indices;

	TextureClass* m_Texture;

	// movement, scaling and rotation vars
	float posX;
	float posY;
	float posZ;

	float roll;
	float pitch;
	float yaw;

	float scaleX;
	float scaleY;
	float scaleZ;
};

#endif
