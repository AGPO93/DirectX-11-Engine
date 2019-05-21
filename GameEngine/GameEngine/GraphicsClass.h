#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// INCLUDES

#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"
#include "Textureshaderclass.h"

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
	void MoveCube(int index);
	void ChangeCube();
	void StartMovement(int index);

	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;

	XMFLOAT3 camRot;
	XMFLOAT3 camPos;
	float moveX = 0.0f;
	float moveY = 2.1f;
	float moveZ = 0.0f;
	int controlCubeIndex = 100;
	bool startMove = false;
};

#endif