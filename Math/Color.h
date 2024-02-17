#pragma once

// カラー
class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	Color()
		: r(0.0f)
		, g(0.0f)
		, b(0.0f)
		, a(0.0f)
	{}

	Color(float r, float g, float b, float a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}

	static const Color kBlack;
	static const Color kWhite;
	static const Color kRed;
	static const Color kGreen;
	static const Color kBlue;
	static const Color kCyan;
	static const Color kMagenta;
	static const Color kYellow;
};

inline Color operator-(const Color& c)
{
	return Color(-c.r, -c.g, -c.b, -c.a);
}

inline Color operator+(const Color& c1, const Color& c2)
{
	return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
}

inline Color operator-(const Color& c1, const Color& c2)
{
	return Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
}

inline Color operator*(const Color& c1, const Color& c2)
{
	return Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
}

inline Color operator*(const Color& c, float a)
{
	return Color(c.r * a, c.g * a, c.b * a, c.a * a);
}

inline Color operator*(float a, const Color& c)
{
	return Color(a * c.r, a * c.g, a * c.b, a * c.a);
}

inline Color operator/(const Color& c, float a)
{
	float oneOverA = 1.0f / a;
	return Color(c.r * oneOverA, c.g * oneOverA, c.b * oneOverA, c.a * oneOverA);
}

inline Color& operator+=(Color& c1, const Color& c2)
{
	c1 = c1 + c2;
	return c1;
}

inline Color& operator-=(Color& c1, const Color& c2)
{
	c1 = c1 - c2;
	return c1;
}

inline Color& operator*=(Color& c1, const Color& c2)
{
	c1 = c1 * c2;
	return c1;
}

inline Color& operator*=(Color& c, float a)
{
	c = c * a;
	return c;
}

inline Color& operator/=(Color& c, float a)
{
	float oneOverA = 1.0f / a;
	c *= oneOverA;
	return c;
}
