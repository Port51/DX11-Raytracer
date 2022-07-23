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
	class Camera;
	class GBuffer;

	struct BufferCB
	{
		uint tileSizeX;
		uint tileSizeY;
		uint tileCountX;
		uint tileCountY;
	};

	class App
	{
	private:
		const uint RaymarchPassCt = 1u;
	public:
		App(const int screenWidth, const int screenHeight, const char* windowTitle, const uint maxThreadCount);
		~App();
	public:
		int Run();
	private:
		void ExecuteTiles(const int startIdx, const int tileCount, const uint iterationIndex);
		void MapImageBuffer(const std::vector<Color>& buffer);
	private:
		std::unique_ptr<Graphics> m_pGfx;
		std::unique_ptr<GBuffer> m_pGBuffer;
		ComPtr<ID3D11Buffer> m_pImageBuffer;
		ComPtr<ID3D11ShaderResourceView> m_pImageBufferSRV;
		ComPtr<ID3D11Buffer> m_pConstantBuffer;
		ComPtr<ID3D11ComputeShader> m_pComputeShader;
		std::unique_ptr<FullScreenBlit> m_pFullScreenBlit;
		std::unique_ptr<CPURaytracer> m_pCPURaytracer;
		std::unique_ptr<Camera> m_pCamera;

		const uint m_maxThreadCount;
		const int m_screenWidth;
		const int m_screenHeight;
	};
}