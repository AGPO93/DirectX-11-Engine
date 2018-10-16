#include "DXApp.h"
#include "Model.h"

class TestApp : public DXApp
{
public:
	TestApp(HINSTANCE hInstance);
	~TestApp();

	bool Init() override;
	void Update(float dt) override;
	void Render(float dt) override;
};

TestApp::TestApp(HINSTANCE hInstance) : DXApp(hInstance)
{
}

TestApp::~TestApp()
{
}

bool TestApp::Init()
{
	return DXApp::Init();
}

void TestApp::Update(float dt)
{
}

void TestApp::Render(float dt)
{
	//bind render target
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

	//set viewport
	auto viewport = CD3D11_VIEWPORT(0.f, 0.f, 800.f, 600.f);
	m_pDeviceContext->RSSetViewports(1, &viewport);

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);

	m_pSwapChain->Present(0, 0);
}

//int main()
//{
//	return 0;
//}

int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd)
{
	Model model;

	TestApp tApp(hInstance);

	//model.Draw();

	if (!tApp.Init()) return 1;

	return tApp.Run();
}

