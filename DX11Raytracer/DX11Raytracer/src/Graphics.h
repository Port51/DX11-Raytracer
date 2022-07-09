#pragma once
#include "CommonHeader.h"
#include <windows.h>
#include "DX11Include.h"

namespace gfx
{
	class Graphics
	{
	public:
		Graphics(const int screenWidth, const int screenHeight, const char* windowTitle);
		~Graphics();
	public:
		void ClearRenderTarget(const float r, const float g, const float b, const float a) const;
		void SetViewport(const int x, const int y, const int width, const int height) const;
	public:
		ID3D11Device1* m_pDevice;
		ID3D11DeviceContext1* m_pDeviceContext;
		IDXGIFactory2* m_pDxgiFactory;
		HWND m_pWindow;
		IDXGISwapChain1* m_pSwapChain;
		ID3D11Texture2D* m_pFrameBuffer;
		ID3D11RenderTargetView* m_pFrameBufferView;
	};
}