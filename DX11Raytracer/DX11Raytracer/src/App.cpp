#include "App.h"
#include "Graphics.h"
#include <assert.h>
#include "ExceptionHandling.h"
#include "FullScreenBlit.h"
#include "CPURaytracer.h"
#include "RayReceiver.h"
#include "Camera.h"
#include "GBuffer.h"
#include <thread>

namespace gfx
{
	App::App(const int screenWidth, const int screenHeight, const char* windowTitle, const uint maxThreadCount)
		: m_pGfx(std::make_unique<Graphics>(screenWidth, screenHeight, windowTitle)),
        m_pGBuffer(std::make_unique<GBuffer>(screenWidth, screenHeight)),
        m_screenWidth(screenWidth),
        m_screenHeight(screenHeight),
        m_maxThreadCount(maxThreadCount)
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

        D3D11_BUFFER_DESC cbd;
        ZERO_MEM(cbd);
        cbd.ByteWidth = sizeof(BufferCB);
        cbd.StructureByteStride = sizeof(BufferCB);
        cbd.Usage = D3D11_USAGE_IMMUTABLE;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0u;
        cbd.MiscFlags = 0u;

        // Initial data for CB
        BufferCB constantBuffer = { TileSize, TileSize, TileDimensionX, TileDimensionY };
        D3D11_SUBRESOURCE_DATA sd;
        ZERO_MEM(sd);
        sd.pSysMem = (void*)&constantBuffer;

        THROW_IF_FAILED(m_pGfx->m_pDevice->CreateBuffer(&cbd, &sd, &m_pConstantBuffer));

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

        vec3 camWS = vec3(9, 2.5, 2.5);
        //camWS = vec3(0, 20, 0.5);
        vec3 targetWS = vec3(0, 0, 0);
        const auto aperture = 0.1;
        const auto focusDist = (camWS - targetWS).Length();
        m_pCamera = std::make_unique<Camera>(camWS, targetWS, vec3(0, 1, 0), 45, AspectRatio, aperture, focusDist);
    }

    App::~App()
    {}

	int App::Run()
	{
        uint nextTileIndex = 0u;
        uint iterationIndex = 0u;

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

            ExecuteTiles(nextTileIndex, m_maxThreadCount, iterationIndex);
            nextTileIndex = (nextTileIndex + m_maxThreadCount);
            if (nextTileIndex >= TileCount)
            {
                nextTileIndex -= TileCount;
                iterationIndex++;
            }

            MapImageBuffer();

            m_pGfx->SetViewport(0, 0, m_screenWidth, m_screenHeight);
            m_pFullScreenBlit->Execute(*m_pGfx, m_pImageBufferSRV.Get(), m_pConstantBuffer.Get());

            THROW_IF_FAILED(m_pGfx->m_pSwapChain->Present(1, 0));
        }
        return 0;
	}

    void App::ExecuteTiles(const int startIdx, const int tileCount, const uint iterationIndex)
    {
        if (UseThreading)
        {
            std::vector<std::thread> renderThreads;
            for (int i = 0; i < tileCount; ++i)
            {
                uint tileIteration = iterationIndex;
                uint idx = (startIdx + i) % TileCount;
                if (idx > TileCount)
                {
                    idx -= TileCount;
                    tileIteration++;
                }
                const uint tx = idx % TileDimensionX;
                const uint ty = idx / TileDimensionX;

                renderThreads.push_back(std::thread(&CPURaytracer::RunTile, m_pCPURaytracer.get(), *m_pCamera.get(), m_pGBuffer->CameraColor.data(), tx, ty, tileIteration, *m_pGBuffer.get(), 0u));
            }

            for (int i = 0; i < tileCount; ++i)
            {
                if (renderThreads.at(i).joinable())
                {
                    renderThreads.at(i).join();
                }
            }
        }
        else
        {
            for (int i = 0; i < tileCount; ++i)
            {
                uint tileIteration = iterationIndex;
                uint idx = (startIdx + i) % TileCount;
                if (idx > TileCount)
                {
                    idx -= TileCount;
                    tileIteration++;
                }
                const uint tx = idx % TileDimensionX;
                const uint ty = idx / TileDimensionX;

                m_pCPURaytracer->RunTile(*m_pCamera.get(), m_pGBuffer->CameraColor.data(), tx, ty, iterationIndex, *m_pGBuffer.get(), 0u);
            }
        }
    }

    void App::MapImageBuffer()
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        m_pGfx->m_pDeviceContext->Map(m_pImageBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

        float* sd = reinterpret_cast<float*>(mappedSubresource.pData);
        memcpy(sd, m_pGBuffer->CameraColor.data(), sizeof(m_pGBuffer->CameraColor.at(0)) * m_pGBuffer->CameraColor.size()); // todo: only map what's changed

        m_pGfx->m_pDeviceContext->Unmap(m_pImageBuffer.Get(), 0);
    }
}