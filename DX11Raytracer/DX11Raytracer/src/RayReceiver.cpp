#include "RayReceiver.h"
#include "Ray.h"
#include "Material.h"

namespace gfx
{
	RayReceiver::RayReceiver(const vec3f positionWS) : m_positionWS(positionWS)
	{
		m_prevFramePositionWS = m_positionWS + vec3f(0.f, 0.001f, 0.f); // testing motion blur...
	}

	const vec3f& RayReceiver::GetCurrentPosition() const
	{
		return m_positionWS;
	}

	const vec3f& RayReceiver::GetPreviousFramePosition() const
	{
		return m_prevFramePositionWS;
	}

	const vec3f RayReceiver::GetPositionAtTime(const f32 time) const
	{
		return m_positionWS * time + m_prevFramePositionWS * (1.0f - time);
	}
}