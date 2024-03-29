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
		void ClearRenderTarget(const f32 r, const f32 g, const f32 b, const f32 a) const;
		void SetViewport(const int x, const int y, const int width, const int height) const;
		ID3D11DeviceContext1& GetContext() const;
	public:
		ComPtr<ID3D11Device1> m_pDevice;
		ComPtr<ID3D11DeviceContext1> m_pDeviceContext;
		ComPtr<IDXGIFactory2> m_pDxgiFactory;
		ComPtr<IDXGISwapChain1> m_pSwapChain;
		ComPtr<ID3D11Texture2D> m_pFrameBuffer;
		ComPtr<ID3D11RenderTargetView> m_pFrameBufferView;
		HWND m_pWindow;
	};
}