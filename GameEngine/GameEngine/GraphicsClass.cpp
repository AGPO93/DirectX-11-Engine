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
	m_Camera->SetPosition(58.0f, 50.0f, 21.0f);
	m_Camera->SetRotation(39.0f, -85.0f, 1.0f);

	camRot = m_Camera->GetRotation();
	camPos = m_Camera->GetPosition();

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
	m_Model->setRotation(50.0f, 50.0f, 0.0f);
	// moves the cube to mouse position, values too high
	//m_Model->setTransform(mouseX/50.0f, -mouseY/50.0f, 0.0f);

	m_Model->UpdateMatrices();
	m_Model->updateInstancesBuffer(m_Direct3D->GetDevice());

	for (int i = 0; i < 10; i++)
	{
		StartMovement(controlCubeIndex + i);
		MoveCube(controlCubeIndex + i);
	}

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
		m_Model->Pathfinding();

		if (!moveCheck)
			moveCheck = true;
		break;
	}
}

void GraphicsClass::CamController(char keyPressed)
{
	m_Camera->SetPosition(camPos.x, camPos.y, camPos.z);
	m_Camera->SetRotation(camRot.x, camRot.y, camRot.z);
	
	switch (keyPressed)
	{
	case 'W':
		camPos.x -= 1;
		break;

	case 'A':
		camPos.z -= 1;
		break;

	case 'S':
		camPos.x += 1;
		break;

	case 'D':
		camPos.z += 1;
		break;

	case 'Q':
		camPos.y += 1;
		break;

	case 'E':
		camPos.y -= 1;
		break;

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

void GraphicsClass::MoveCube(int index)
{
	if (moveCheck)
	{
		m_Model->MoveInstance(index, moveX, moveY, moveZ);
	}
}

void GraphicsClass::ChangeCube() //not being used atm.
{
	if (controlCubeIndex == 3)
		controlCubeIndex = 0;
	else
		controlCubeIndex++;

	XMFLOAT3 previousPos = m_Model->GetCurrentPos(controlCubeIndex);
	moveX = previousPos.x;
	moveY = previousPos.y;
	moveZ = previousPos.z;
}

void GraphicsClass::StartMovement(int index)
{
	if (startMove)
	{
		XMFLOAT3 previousPos = m_Model->GetCurrentPos(index);
		moveX = previousPos.x;
		moveY = previousPos.y;
		moveZ = previousPos.z;

		// Get destination node
		ModelClass::NodeType node;

		if (!m_Model->instances[index].reachedGoal)
		{
			node = m_Model->realPath[m_Model->instances[index].goalNodeIndex];
		}
		else
		{
			// Set goal node
			// If last soldier, set as final node
			if (index - controlCubeIndex == 3)
			{
				node = m_Model->realPath[m_Model->instances[index].goalNodeIndex];
			}
			// If cube index is less than neighbour array size, set as neighbour node
			else if (m_Model->realPath[m_Model->instances[index].goalNodeIndex].neighbours.size() > index - controlCubeIndex)
			{
				node = *m_Model->realPath[m_Model->instances[index].goalNodeIndex].neighbours[index - controlCubeIndex];
			}
			// else set as previous node
			else
			{
				node = m_Model->realPath[m_Model->instances[index].goalNodeIndex - 2];
			}
		}

		if (m_Model->instances[index].position.x != node.position.x)
		{
			if (node.position.x > m_Model->instances[index].position.x)
			{
				moveX += 0.5f;
			}
			else if (node.position.x < m_Model->instances[index].position.x)
			{
				moveX -= 0.5f;
			}
		}

		if (m_Model->instances[index].position.z != node.position.z)
		{
			if (node.position.z > m_Model->instances[index].position.z)
			{
				moveZ += 0.5f;
			}
			else if (node.position.z < m_Model->instances[index].position.z)
			{
				moveZ -= 0.5f;
			}
		}
			
		// Update node data
		if (m_Model->instances[index].position.x == node.position.x &&
			m_Model->instances[index].position.z == node.position.z)
		{
			if (m_Model->instances[index].goalNodeIndex < m_Model->realPath.size() - 1)
			{
				m_Model->instances[index].goalNodeIndex++;
			}
			else
			{
				// If at last node, go to goal node
				m_Model->instances[index].reachedGoal = true;
			}
		}
	}
}

