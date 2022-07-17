#include "FullScreenBlit.h"
#include "DX11Include.h"
#include "ExceptionHandling.h"
#include "Graphics.h"

namespace gfx
{
	FullScreenBlit::FullScreenBlit(Graphics& gfx)
	{
        // Read pre-compiled vertex shader
        ID3DBlob* vsBlob;
        THROW_IF_FAILED(D3DReadFileToBlob(L"Assets/FullScreenVS.cso", &vsBlob));

        THROW_IF_FAILED(gfx.m_pDevice->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            &m_pVertexShader
        ));

		// Setup vertex buffer
		// Just pass indices and figure out NDC + UVs in vertex shader
		D3D11_BUFFER_DESC bd;
		ZERO_MEM(bd);

		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = 3u * sizeof(uint16_t);
		bd.StructureByteStride = sizeof(uint16_t);

		uint16_t vertexData[3] = { 0u, 1u, 2u };

		D3D11_SUBRESOURCE_DATA sd;
		ZERO_MEM(sd);
		sd.pSysMem = vertexData;
		sd.SysMemPitch = 0;			// only for 2D/3D textures
		sd.SysMemSlicePitch = 0;	// only for 3D textures

		gfx.m_pDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer);

        m_VertexBufferStrides.resize(1u, bd.StructureByteStride);
        m_VertexBufferOffsets.resize(1u, 0u);

        // Read pre-compiled pixel shader
        ID3DBlob* psBlob;
        THROW_IF_FAILED(D3DReadFileToBlob(L"Assets/FullScreenPS.cso", &psBlob));

        THROW_IF_FAILED(gfx.m_pDevice->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            &m_pPixelShader
        ));

        D3D11_SAMPLER_DESC spd;
        ZERO_MEM(spd);
        spd.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
        spd.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
        spd.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
        spd.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
        gfx.m_pDevice->CreateSamplerState(&spd, &m_pSamplerState);

        D3D11_RASTERIZER_DESC rsd;
        ZERO_MEM(rsd);
        rsd.ScissorEnable = false;
        rsd.CullMode = D3D11_CULL_NONE;
        rsd.MultisampleEnable = false;
        rsd.AntialiasedLineEnable = false;
        rsd.FillMode = D3D11_FILL_SOLID;
        rsd.FrontCounterClockwise = true;

        THROW_IF_FAILED(gfx.m_pDevice->CreateRasterizerState(&rsd, &m_pRasterizerState));

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "INDEX", 0, DXGI_FORMAT_R16_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        THROW_IF_FAILED(gfx.m_pDevice->CreateInputLayout(layout, 1u, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_pInputLayout));

        D3D11_DEPTH_STENCIL_DESC dsd;
        ZERO_MEM(dsd);
        dsd.DepthEnable = false;
        dsd.StencilEnable = false;

        THROW_IF_FAILED(gfx.m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencilState));
	}

    void FullScreenBlit::Execute(Graphics& gfx, ID3D11ShaderResourceView* const bufferSRV, ID3D11Buffer* const pConstantBuffer) const
    {
        gfx.m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        gfx.m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
        gfx.m_pDeviceContext->IASetVertexBuffers(0, 1u, m_pVertexBuffer.GetAddressOf(), m_VertexBufferStrides.data(), m_VertexBufferOffsets.data());
        gfx.m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
        gfx.m_pDeviceContext->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
        gfx.m_pDeviceContext->RSSetState(m_pRasterizerState.Get());
        gfx.m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
        gfx.m_pDeviceContext->PSSetSamplers(0, 1u, m_pSamplerState.GetAddressOf());
        gfx.m_pDeviceContext->PSSetShaderResources(0u, 1u, &bufferSRV);
        gfx.m_pDeviceContext->PSSetConstantBuffers(0u, 1u, &pConstantBuffer);
        gfx.m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0u);
        gfx.m_pDeviceContext->OMSetRenderTargets(1u, gfx.m_pFrameBufferView.GetAddressOf(), nullptr);
        gfx.m_pDeviceContext->Draw(3u, 0u);
    }
}