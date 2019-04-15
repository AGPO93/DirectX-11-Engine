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

private:
	void MoveCube();
	void ChangeCube();

	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;

	XMFLOAT3 camPos;
	float rotation = 0;
	float moveX = 0;
	float moveY = 0;
	float moveZ = 0;
	int cubeIndex = 0;
};

#endif