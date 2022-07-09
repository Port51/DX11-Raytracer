#include "ExceptionHandling.h"

namespace gfx
{

	GfxException::GfxException(size_t line, const char* file, HRESULT hr)
		: m_line(line), m_file(file)
	{
		std::ostringstream ss;
		ss << "[Error]\t ";
		ss << "[File]\t " << file << std::endl;
		ss << "[Line]\t " << line << std::endl;
		ss << "[Code]\t " << HrToStringCode(hr) << " (" << std::to_string(hr) << ")" << std::endl;
		ss << "[Desc]\t " << HrToMessageString(hr) << std::endl;
		m_msg = ss.str();
	}

	GfxException::GfxException(size_t line, const char* file, std::string msg)
		: m_line(line), m_file(file)
	{
		std::ostringstream ss;
		ss << "[Error]\t " << msg << std::endl;
		ss << "[File]\t " << file << std::endl;
		ss << "[Line]\t " << line << std::endl;
		m_msg = ss.str();
	}

	GfxException::GfxException(size_t line, const char* file, const char* msg)
		: GfxException(line, file, std::string(msg))
	{}

	const char* GfxException::what() const
	{
		return m_msg.c_str();
	}

	std::string GfxException::HrToMessageString(HRESULT hr)
	{
		return std::system_category().message(hr);
	}

	std::string GfxException::HrToStringCode(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}

	std::string GfxException::GetExceptionString(int line, const char* file, HRESULT hr)
	{
		std::ostringstream oss;
		oss << "[Error Code] " << HrToStringCode(hr) << " (" << std::to_string(hr) << ")" << std::endl << std::endl
			<< "[File] " << std::string(file) << std::endl
			<< "[Line] " << std::to_string(line) << std::endl << std::endl
			<< "[Description]" << HrToMessageString(hr);
		return oss.str();
	}

}