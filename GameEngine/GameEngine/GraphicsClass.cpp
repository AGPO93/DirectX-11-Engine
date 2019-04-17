#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}
	
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(20.0f, 15.0f, -25.0f);
	m_Camera->SetRotation(15.0f, 4.0f, 0.0f);

	camRot = m_Camera->GetRotation();

	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	
	return;
}

bool GraphicsClass::Frame()
{
	m_Model->setRotation(1.0f, rotation, 1.0f);
	// moves the cube to mouse position, values too high
	//m_Model->setTransform(mouseX/50.0f, -mouseY/50.0f, 0.0f);

	m_Model->UpdateMatrices();
	m_Model->updateInstancesBuffer(m_Direct3D->GetDevice());
	StartMovement();

	bool result;
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX modelMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	modelMatrix = m_Model->GetModelMatrix();
	//modelMatrix = XMMatrixRotationY(rotation); 

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(),
		m_Model->GetVertexCount(), m_Model->GetInstanceCount(),
		m_Model->GetIndexCount(), modelMatrix, viewMatrix, 
		projectionMatrix);

	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void GraphicsClass::CubeController(char keyPressed)
{
	switch (keyPressed)
	{
	/*case 'W':
		moveZ += .25f;
		if (!moveCheck)
			moveCheck = true;
		break;

	case 'A':
		moveX -= .25f;
		if (!moveCheck)
			moveCheck = true;
		break;

	case 'S':
		moveZ -= .25f;
		if (!moveCheck)
			moveCheck = true;
		break;

	case 'D':
		moveX += .25f;
		if (!moveCheck)
			moveCheck = true;
		break;*/

	case 'C':
		//ChangeCube();
		startMove = true;

		if (!moveCheck)
			moveCheck = true;
		break;
	}

	MoveCube();
}

void GraphicsClass::CamController(char keyPressed)
{
	//m_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	m_Camera->SetRotation(camRot.x, camRot.y, camRot.z);
	
	switch (keyPressed)
	{
	case 'I':
		camRot.x -= 1;
		break;

	case 'J':
		camRot.y -= 1;
		break;

	case 'K':
		camRot.x += 1;
		break;

	case 'L':
		camRot.y += 1;
		break;
	}
}

void GraphicsClass::MoveCube()
{
	if (moveCheck)
	{
		m_Model->MoveInstance(cubeIndex, moveX, moveY, moveZ);
	}
}

void GraphicsClass::ChangeCube()
{
	if (cubeIndex == 3)
		cubeIndex = 0;
	else
		cubeIndex++;

	XMFLOAT3 previousPos = m_Model->GetCurrentPos(cubeIndex);
	moveX = previousPos.x;
	moveY = previousPos.y;
	moveZ = previousPos.z;
}

void GraphicsClass::StartMovement()
{
	if (startMove)
	{
		if (m_Model->nodes[goalNode].position.x > m_Model->instances[cubeIndex].position.x)
		{
			moveX += .25f;
		}
		else if (m_Model->nodes[goalNode].position.x < m_Model->instances[cubeIndex].position.x)
		{
			moveX -= .25f;
		}
		else if (m_Model->nodes[goalNode].position.z > m_Model->instances[cubeIndex].position.z)
		{
			moveZ += .25f;
		}
		else if (m_Model->nodes[goalNode].position.z < m_Model->instances[cubeIndex].position.z)
		{
			moveX -= .25f;
		}
	}
}

