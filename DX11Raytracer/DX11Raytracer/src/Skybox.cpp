#include "Skybox.h"

namespace gfx
{
	Skybox::Skybox(const Color& upwardColor, const Color& horizonColor, const Color& downwardColor)
		: m_upwardColor(upwardColor), m_horizonColor(horizonColor), m_downwardColor(downwardColor)
	{}

	const Color Skybox::GetColor(const vec3 & direction)
	{
		const vec3 directionNorm = Normalize(direction);
		const double vertical = std::abs(directionNorm.y);

		const double upwardLerp = std::pow(vertical, 0.25);
		const Color upwardColor = (1.0 - upwardLerp) * m_horizonColor + upwardLerp * m_upwardColor;

		const double downwardLerp = std::pow(vertical, 0.09);
		const Color downwardColor = (1.0 - downwardLerp) * m_horizonColor + downwardLerp * m_downwardColor;

		return (directionNorm.y > 0.0) ? upwardColor : downwardColor;
	}
}