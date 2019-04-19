#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// INCLUDES

#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"

// GLOBALS 
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();	
	void CubeController(char keyPressed);
	void CamController(char dir);

	bool moveCheck = false;

private:
	void MoveCube();
	void ChangeCube();
	void StartMovement();

	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;

	XMFLOAT3 camRot;
	float rotation = 0;
	float moveX = 0.0f;
	float moveY = 2.1f;
	float moveZ = 0.0f;
	int controlCubeIndex = 100;
	int goalNodeIndex = 0; // use model -> goalNode
	bool startMove = false;
};

#endif