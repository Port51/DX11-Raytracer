#pragma once
#include "CommonHeader.h"
#include <vector>

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11ComputeShader;

namespace gfx
{
	class Graphics;
	class FullScreenBlit;
	class CPURaytracer;

	class App
	{
	public:
		App(const int screenWidth, const int screenHeight, const char* windowTitle);
		~App();
	public:
		int Run();
	private:
		void ExecuteTileRow(const int rowIdx);
		void MapImageBuffer();
	private:
		std::unique_ptr<Graphics> m_pGfx;
		ComPtr<ID3D11Buffer> m_pImageBuffer;
		ComPtr<ID3D11ShaderResourceView> m_pImageBufferSRV;
		std::vector<Color> m_imageData;
		ComPtr<ID3D11ComputeShader> m_pComputeShader;
		std::unique_ptr<FullScreenBlit> m_pFullScreenBlit;
		std::unique_ptr<CPURaytracer> m_pCPURaytracer;

		const int m_screenWidth;
		const int m_screenHeight;
	};
}