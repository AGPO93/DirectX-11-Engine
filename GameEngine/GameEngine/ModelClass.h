#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

// INCLUDES 
#include <vector>
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
using namespace std;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct InstanceType
	{
		XMFLOAT3 position;
		XMMATRIX instanceMatrix;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	void UpdateMatrices();
	void setRotation(float newPitch, float newYaw, float newRoll);
	void setTransform(float newPosX, float newPosY, float newPosZ);
	void MoveInstance(int i, float newPosX, float newPosY, float newPosZ);
	XMFLOAT3 GetCurrentPos(int i);
	bool updateInstancesBuffer(ID3D11Device* );
	int GetVertexCount() { return m_vertexCount; }
	int GetInstanceCount() { return m_instanceCount; }
	int GetIndexCount() { return m_indexCount; }
	XMMATRIX GetModelMatrix() {return  ModelMatrix;}

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	void DrawGrid();
	void LoadArrays(VertexType* vertices, unsigned long* indices);

	ID3D11Buffer *m_vertexBuffer, *m_constantBuffer, *m_instanceBuffer,
				 *m_indexBuffer;

	XMMATRIX TransformMatrix;
	XMMATRIX ScaleMatrix;
	XMMATRIX RotationMatrix;
	XMMATRIX FudgeMatrix;
	XMMATRIX ModelMatrix;

	vector<InstanceType> instances;

	int m_vertexCount, m_instanceCount, m_indexCount;

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
