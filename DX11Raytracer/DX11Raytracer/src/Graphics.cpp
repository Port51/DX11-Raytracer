#include "Graphics.h"
#include <stdexcept>
#include "ExceptionHandling.h"

namespace gfx
{
	Graphics::Graphics(const int screenWidth, const int screenHeight, const char* windowTitle)
	{
        // Create window
        {
            WNDCLASSEXA wndClassEx = { sizeof(wndClassEx) };
            wndClassEx.lpfnWndProc = DefWindowProcA;
            wndClassEx.lpszClassName = windowTitle;
            wndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

            RegisterClassExA(&wndClassEx);

            RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };

            // Get larger size so interior region is requested size
            if (!AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE))
            {
                throw std::runtime_error("Failed to adjust window rect");
            }

            //m_pWindow = CreateWindowExA(0, windowTitle, windowTitle, WS_POPUP | WS_MAXIMIZE | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);
            m_pWindow = CreateWindowExA(0, windowTitle, windowTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, nullptr, nullptr);
        }
        
        // Setup DX11
        {
            uint creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
            // If the project is in a debug build, enable the debug layer.
            creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

            // Setup device
            ID3D11Device* baseDevice;
            ID3D11DeviceContext* baseDeviceContext;
            THROW_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, nullptr, &baseDeviceContext));

            THROW_IF_FAILED(baseDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(m_pDevice.GetAddressOf())));

            THROW_IF_FAILED(baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(m_pDeviceContext.GetAddressOf())));

            IDXGIDevice1* dxgiDevice;
            THROW_IF_FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&dxgiDevice)));

            IDXGIAdapter* dxgiAdapter;
            THROW_IF_FAILED(dxgiDevice->GetAdapter(&dxgiAdapter));

            THROW_IF_FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(m_pDxgiFactory.GetAddressOf())));
        }
        
        // Setup swap chain
        {
            DXGI_SWAP_CHAIN_DESC1 scd;
            ZERO_MEM(scd);
            scd.Width = 0; // use window width
            scd.Height = 0; // use window height
            scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            scd.Stereo = FALSE;
            scd.SampleDesc.Count = 1;
            scd.SampleDesc.Quality = 0;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
            scd.BufferCount = 2;
            scd.Scaling = DXGI_SCALING_STRETCH;
            scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            scd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            scd.Flags = 0u;

            THROW_IF_FAILED(m_pDxgiFactory->CreateSwapChainForHwnd(m_pDevice.Get(), m_pWindow, &scd, nullptr, nullptr, m_pSwapChain.GetAddressOf()));

            THROW_IF_FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pFrameBuffer.GetAddressOf())));

            THROW_IF_FAILED(m_pDevice->CreateRenderTargetView(m_pFrameBuffer.Get(), nullptr, m_pFrameBufferView.GetAddressOf()));

            //D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.0f, 1.0f };
        }
	}
	
    Graphics::~Graphics()
	{
        DestroyWindow(m_pWindow);
    }

    void Graphics::ClearRenderTarget(const float r, const float g, const float b, const float a) const
    {
        static float colorArray[4];
        colorArray[0] = r;
        colorArray[1] = g;
        colorArray[2] = b;
        colorArray[3] = a;
        m_pDeviceContext->ClearRenderTargetView(m_pFrameBufferView.Get(), colorArray);
    }

    void Graphics::SetViewport(const int x, const int y, const int width, const int height) const
    {
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0;
        vp.MaxDepth = 1;
        vp.TopLeftX = (FLOAT)x;
        vp.TopLeftY = (FLOAT)y;
        m_pDeviceContext->RSSetViewports(1u, &vp);
    }
}