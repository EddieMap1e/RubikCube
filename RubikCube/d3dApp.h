#pragma once

#include "d3dUtil.h"
#include <DirectXMath.h>
#include "GameTimer.h"
#include "Mouse.h"
#include "Keyboard.h"

// �������Ҫ���õĿ�
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);    // �ڹ��캯���ĳ�ʼ���б�Ӧ�����úó�ʼ����
	virtual ~D3DApp();

	HINSTANCE AppInst()const;       // ��ȡӦ��ʵ���ľ��
	HWND      MainWnd()const;       // ��ȡ�����ھ��
	float     AspectRatio()const;   // ��ȡ��Ļ��߱�

	int Run();                      // ���г���ִ����Ϣ�¼���ѭ��

	// ��ܷ������ͻ���������Ҫ������Щ������ʵ���ض���Ӧ������
	virtual bool Init();            // �ø��෽����Ҫ��ʼ�����ڡ�Direct2D��Direct3D����
	virtual void OnResize();        // �ø��෽����Ҫ�ڴ��ڴ�С�䶯��ʱ�����
	virtual void UpdateScene(float dt) = 0;   // ������Ҫʵ�ָ÷��������ÿһ֡�ĸ���
	virtual void DrawScene() = 0;             // ������Ҫʵ�ָ÷��������ÿһ֡�Ļ���
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ���ڵ���Ϣ�ص�����
protected:
	bool InitMainWindow();      // ���ڳ�ʼ��
	bool InitDirect3D();        // Direct3D��ʼ��

	void CalculateFrameStats(); // ����ÿ��֡�����ڴ�����ʾ

protected:

	HINSTANCE mhAppInst;        // Ӧ��ʵ�����
	HWND      mhMainWnd;        // �����ھ��
	bool      mAppPaused;       // Ӧ���Ƿ���ͣ
	bool      mMinimized;       // Ӧ���Ƿ���С��
	bool      mMaximized;       // Ӧ���Ƿ����
	bool      mResizing;        // ���ڴ�С�Ƿ�仯
	bool	  mEnable4xMsaa;	// �Ƿ���4�����ز���
	UINT      m4xMsaaQuality;   // MSAA֧�ֵ������ȼ�

	GameTimer mTimer;           // ��ʱ��

	// Direct3D 11
	ComPtr<ID3D11Device> md3dDevice;							// D3D11�豸
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;			// D3D11�豸������
	ComPtr<IDXGISwapChain> mSwapChain;							// D3D11������
	// Direct3D 11.1
	ComPtr<ID3D11Device1> md3dDevice1;							// D3D11.1�豸
	ComPtr<ID3D11DeviceContext1> md3dImmediateContext1;			// D3D11.1�豸������
	ComPtr<IDXGISwapChain1> mSwapChain1;						// D3D11.1������
	// ������Դ
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;				// ���ģ�建����
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;			// ��ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;			// ���ģ����ͼ
	D3D11_VIEWPORT mScreenViewport;                             // �ӿ�
	// ��������
	std::unique_ptr<DirectX::Mouse> mMouse;						// ���
	DirectX::Mouse::ButtonStateTracker mMouseTracker;			// ���״̬׷����
	std::unique_ptr<DirectX::Keyboard> mKeyboard;				// ����
	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker;	// ����״̬׷����


	wstring mMainWndCaption;									// �����ڱ���
	int mClientWidth;                                           // �ӿڿ��
	int mClientHeight;                                          // �ӿڸ߶�
};