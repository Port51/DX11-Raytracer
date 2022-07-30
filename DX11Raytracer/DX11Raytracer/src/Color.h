#pragma once

namespace gfx
{
	struct Color
	{
	public:
		f32 r;
		f32 g;
		f32 b;
		f32 a;

		Color()
			: r(0.f), g(0.f), b(0.f), a(0.f)
		{}

		Color(const f32 v)
			: r(v), g(v), b(v), a(v)
		{}

		Color(const f32 r, const f32 g, const f32 b)
			: r(r), g(g), b(b), a(1.f)
		{}

		Color(const f32 r, const f32 g, const f32 b, const f32 a)
			: r(r), g(g), b(b), a(a)
		{}

		Color& operator+=(const Color& v)
		{
			r += v.r;
			g += v.g;
			b += v.b;
			a += v.a;
			return *this;
		}

		Color& operator*=(const f32 t)
		{
			r += t;
			g += t;
			b += t;
			a += t;
			return *this;
		}

		Color& operator/=(const f32 t)
		{
			return *this *= 1 / t;
		}

		static Color Random()
		{
			return Color(Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat());
		}

		static Color Random(const f32 min, const f32 max)
		{
			return Color(Random::RandomFloat(min, max), Random::RandomFloat(min, max), Random::RandomFloat(min, max), Random::RandomFloat(min, max));
		}
	};

	inline std::ostream& operator<<(std::ostream& out, const Color& v)
	{
		return out << '[' << v.r << ' ' << v.g << ' ' << v.b << ' ' << v.a << ']';
	}

	inline Color operator+(const Color& u, const Color& v)
	{
		return Color(u.r + v.r, u.g + v.g, u.b + v.b, u.a + v.a);
	}

	inline Color operator-(const Color& u, const Color& v)
	{
		return Color(u.r - v.r, u.g - v.g, u.b - v.b, u.a - v.a);
	}

	inline Color operator*(const Color& u, const Color& v)
	{
		return Color(u.r * v.r, u.g * v.g, u.b * v.b, u.a * v.a);
	}

	inline Color operator*(f32 t, const Color& v)
	{
		return Color(t * v.r, t * v.g, t * v.b, t * v.a);
	}

	inline Color operator*(const Color& v, f32 t)
	{
		return t * v;
	}

	inline Color operator/(Color v, f32 t)
	{
		return (1 / t) * v;
	}

	inline Color Lerp(const Color& u, const Color& v, const f32 lerp)
	{
		const f32 rcpLerp = 1.f - lerp;
		return Color(u.r * rcpLerp + lerp * v.r,
			u.g * rcpLerp + lerp * v.g,
			u.b * rcpLerp + lerp * v.b,
			u.a * rcpLerp + lerp * v.a);
	}
}