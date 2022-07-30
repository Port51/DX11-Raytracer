#include "Skybox.h"

namespace gfx
{
	Skybox::Skybox(const Color& upwardColor, const Color& horizonColor, const Color& downwardColor)
		: m_upwardColor(upwardColor), m_horizonColor(horizonColor), m_downwardColor(downwardColor)
	{}

	const Color& Skybox::GetColor(const vec3f & direction)
	{
		const vec3f directionNorm = Normalize(direction);
		const f32 vertical = std::abs(directionNorm.y);

		const f32 upwardLerp = std::pow(vertical, 0.25f);
		const Color upwardColor = Lerp(m_horizonColor, m_upwardColor, upwardLerp);

		const f32 downwardLerp = std::pow(vertical, 0.09f);
		const Color downwardColor = Lerp(m_horizonColor, m_downwardColor, downwardLerp);

		return (directionNorm.y > 0.0f) ? upwardColor : downwardColor;
	}

	const Color& Skybox::GetFogColor()
	{
		return m_horizonColor;
	}
}