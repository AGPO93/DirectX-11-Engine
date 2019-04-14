#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

// INCLUDES 
#include <vector>
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
using namespace std;
// Class name: ModelClass
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
		//DirectX::XMVECTOR position;
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
	bool updateInstancesBuffer(ID3D11Device* );
	int GetVertexCount();
	int GetInstanceCount();
	int GetIndexCount();
	XMMATRIX GetModelMatrix();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

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
