#pragma once
#include <stdexcept>
#include <string>
#include <sstream>
#include <windows.h>

namespace gfx
{

	class GfxException : public std::exception
	{
	public:
		GfxException(size_t line, const char* file, HRESULT hr);
		GfxException(size_t line, const char* file, std::string msg = "");
		GfxException(size_t line, const char* file, const char* msg = "");
		const char* what() const override;
	private:
		std::string HrToMessageString(HRESULT hr);
		std::string HrToStringCode(HRESULT hr);
		std::string GetExceptionString(int line, const char* file, HRESULT hr);
	private:
		const size_t m_line;
		const char* m_file;
		std::string m_msg;
	};

}

#define THROW_IF_FAILED(hr) if (FAILED(hr)) { throw GfxException(__LINE__, __FILE__, hr); }
#define THROW_ALWAYS(hr) throw GfxException(__LINE__, __FILE__, hr)
#define THROW(msg) throw GfxException(__LINE__, __FILE__, msg)