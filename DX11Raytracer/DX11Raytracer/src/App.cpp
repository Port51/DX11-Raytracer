#include "App.h"
#include "Graphics.h"
#include <assert.h>
#include "ExceptionHandling.h"
#include "FullScreenBlit.h"
#include "CPURaytracer.h"
#include "RayReceiver.h"
#include "Camera.h"

namespace gfx
{
	App::App(const int screenWidth, const int screenHeight, const char* windowTitle)
		: m_pGfx(std::make_unique<Graphics>(screenWidth, screenHeight, windowTitle)),
        m_screenWidth(screenWidth),
        m_screenHeight(screenHeight)
	{
        D3D11_BUFFER_DESC ibd;
        ZERO_MEM(ibd);
        ibd.ByteWidth = sizeof(Color) * screenWidth * screenHeight;
        ibd.StructureByteStride = sizeof(Color);
        ibd.Usage = D3D11_USAGE_DYNAMIC;
        ibd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ibd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        THROW_IF_FAILED(m_pGfx->m_pDevice->CreateBuffer(&ibd, nullptr, &m_pImageBuffer));
        THROW_IF_FAILED(m_pGfx->m_pDevice->CreateShaderResourceView(m_pImageBuffer.Get(), nullptr, &m_pImageBufferSRV));

        m_imageData.resize(screenWidth * screenHeight);

        // Setup some test data to make sure tiles work correctly
        for (int tx = 0; tx < TileDimensionX; ++tx)
        {
            for (int ty = 0; ty < TileDimensionY; ++ty)
            {
                for (int lx = 0; lx < TileSize; ++lx)
                {
                    for (int ly = 0; ly < TileSize; ++ly)
                    {
                        int x = TileSize * tx + lx;
                        int y = TileSize * ty + ly;
                        float v0 = (float)lx / TileSize;
                        float v1 = (float)ly / TileSize;

                        int tileOffset = (TileSize * TileSize) * (ty * TileDimensionX + tx);
                        int pixelOffset = ly * TileSize + lx;
                        int idx = tileOffset + pixelOffset;

                        m_imageData[idx].r = v0;
                        m_imageData[idx].g = v1;
                        m_imageData[idx].b = (float)x / 256;
                        m_imageData[idx].a = 0;
                    }
                }
            }
        }

        // Read pre-compiled compute shader
        ID3DBlob* csBlob;
        THROW_IF_FAILED(D3DReadFileToBlob(L"Assets/UpdateFrameBuffer.cso", &csBlob));
        
        THROW_IF_FAILED(m_pGfx->m_pDevice->CreateComputeShader(
            csBlob->GetBufferPointer(),
            csBlob->GetBufferSize(),
            nullptr,
            &m_pComputeShader
        ));

        m_pFullScreenBlit = std::make_unique<FullScreenBlit>(*m_pGfx);
        m_pCPURaytracer = std::make_unique<CPURaytracer>();

        vec3 camWS = vec3(13, 2, 3);
        vec3 targetWS = vec3(0, 0, 0);
        auto aperture = 0.1;
        auto focusDist = (camWS - targetWS).Length();
        m_pCamera = std::make_unique<Camera>(camWS, targetWS, vec3(0, 1, 0), 30, AspectRatio, aperture, focusDist);
    }

    App::~App()
    {}

	int App::Run()
	{
        int tx = 0;
        int ty = 0;

        // Render loop
        while (true)
        {
            MSG msg;

            while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_KEYDOWN
                    || msg.message == WM_CLOSE
                    || msg.message == WM_DESTROY) return 0;
                DispatchMessageA(&msg);
            }

            // Update CB
            /*{
                D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                gfx.m_deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

                Constants* constants = reinterpret_cast<Constants*>(mappedSubresource.pData);
                // Update constants here...

                gfx.m_deviceContext->Unmap(constantBuffer, 0);
            }*/

            ExecuteTileRow(ty);
            ty = (ty + 1) % TileDimensionY;

            MapImageBuffer();

            m_pGfx->SetViewport(0, 0, m_screenWidth, m_screenHeight);
            m_pFullScreenBlit->Execute(*m_pGfx, m_pImageBufferSRV.Get());

            THROW_IF_FAILED(m_pGfx->m_pSwapChain->Present(1, 0));
        }
        return 0;
	}

    void App::ExecuteTileRow(const int rowIdx)
    {
        for (int tx = 0; tx < TileDimensionX; ++tx)
        {
            m_pCPURaytracer->RunTile(*m_pCamera.get(), m_imageData.data(), tx, rowIdx);
        }
    }

    void App::MapImageBuffer()
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        m_pGfx->m_pDeviceContext->Map(m_pImageBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

        float* sd = reinterpret_cast<float*>(mappedSubresource.pData);
        memcpy(sd, m_imageData.data(), sizeof(m_imageData.at(0)) * m_imageData.size()); // todo: only map what's changed

        m_pGfx->m_pDeviceContext->Unmap(m_pImageBuffer.Get(), 0);
    }
}