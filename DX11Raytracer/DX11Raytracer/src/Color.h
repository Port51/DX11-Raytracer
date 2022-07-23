#pragma once

namespace gfx
{
	struct Color
	{
	public:
		float r;
		float g;
		float b;
		float a;

		Color()
			: r(0.f), g(0.f), b(0.f), a(0.f)
		{}

		Color(const float v)
			: r(v), g(v), b(v), a(v)
		{}

		Color(const float r, const float g, const float b, const float a)
			: r(r), g(g), b(b), a(a)
		{}

		Color(const double v)
			: r(static_cast<float>(v)), g(static_cast<float>(v)), b(static_cast<float>(v)), a(static_cast<float>(v))
		{}

		Color(const double r, const double g, const double b, const double a)
			: r(static_cast<float>(r)), g(static_cast<float>(g)), b(static_cast<float>(b)), a(static_cast<float>(a))
		{}

		Color& operator+=(const Color& v)
		{
			r += v.r;
			g += v.g;
			b += v.b;
			a += v.a;
			return *this;
		}

		Color& operator*=(const float t)
		{
			r += t;
			g += t;
			b += t;
			a += t;
			return *this;
		}

		Color& operator/=(const float t)
		{
			return *this *= 1 / t;
		}

		static Color Random()
		{
			return Color(Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat());
		}

		static Color Random(const float min, const float max)
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

	inline Color operator*(float t, const Color& v)
	{
		return Color(t * v.r, t * v.g, t * v.b, t * v.a);
	}

	inline Color operator*(const Color& v, float t)
	{
		return t * v;
	}

	inline Color operator/(Color v, float t)
	{
		return (1 / t) * v;
	}
}