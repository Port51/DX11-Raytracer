#pragma once
#include "CommonHeader.h"
#include <vector>

struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;

namespace gfx
{
	class Graphics;

	class FullScreenBlit
	{
	public:
		FullScreenBlit(Graphics& gfx);
	public:
		void Execute(Graphics& gfx, ID3D11ShaderResourceView* const bufferSRV) const;
	private:
		ID3D11InputLayout* m_pInputLayout;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11SamplerState* m_pSamplerState;
		ID3D11RasterizerState* m_pRasterizerState;
		ID3D11DepthStencilState* m_pDepthStencilState;
		std::vector<uint> m_VertexBufferStrides;
		std::vector<uint> m_VertexBufferOffsets;
	};
}